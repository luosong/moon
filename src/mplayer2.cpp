/*
 * mplayer2.cpp: 
 *
 * Authors
 * 	Jeffrey Stedfast <fejj@novell.com>
 *  Rolf Bjarne Kvinge  <RKvinge@novell.com>
 *
 * Copyright 2007 Novell, Inc. (http://www.novell.com)
 *
 * See the LICENSE file included with the distribution for details.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib.h>

#include <poll.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <asoundlib.h>

G_BEGIN_DECLS
#include <stdint.h>
#include <limits.h>
G_END_DECLS

#include "clock.h"
#include "mplayer.h"
#include "pipeline.h"
#include "runtime.h"
#include "list.h"

#ifdef MOON_MEDIA

#if GLIB_SIZEOF_VOID_P == 8
#define ALIGN(addr,size) (uint8_t *) (((uint64_t) (((uint8_t *) (addr)) + (size) - 1)) & ~((size) - 1))
#else
#define ALIGN(addr,size) (uint8_t *) (((uint32_t) (((uint8_t *) (addr)) + (size) - 1)) & ~((size) - 1))
#endif

#define AVCODEC_MAX_AUDIO_FRAME_SIZE 192000 // 1 second of 48khz 32bit audio
#define AUDIO_BUFLEN (AVCODEC_MAX_AUDIO_FRAME_SIZE * 2)

static uint64_t audio_play (Audio *audio, bool play, struct pollfd *ufds, int nfds);
static bool 	audio_decode (Audio *audio);
static void *	audio_loop (void *data);

extern guint32 moonlight_flags;

class Packet : public List::Node {
public:
	IMediaStream* stream;
	int duration;
	int64_t pts;
	size_t size;
	uint8_t *data;
	
	MediaFrame* frame;
	
	Packet (MediaFrame *pkt);
	virtual ~Packet ();
};

Packet::Packet (MediaFrame *pkt)
{
	size_t msize;
	
	stream = pkt->stream;
	duration = pkt->duration;
	size = pkt->uncompressed_size;
	pts = pkt->pts;
	
	// pad to allow 32bit-word sized reads
	msize = (size + 3) & ~0x3;
	
	// add another word as padding
	msize += 4;
	
	data = (uint8_t *) g_malloc0 (msize);
	memcpy (data, pkt->uncompressed_data, size);
	memset (data + size, 0, msize - size);
	frame = pkt;
}

Packet::~Packet ()
{
	//printf ("Packet::~Packet () Deleting packet whose data ptr is: %p.\n", data);
	g_free (data);
	data = NULL;
	
	delete frame;
	frame = NULL;
}


struct Audio {
	Queue *queue;
	
	pthread_mutex_t init_mutex;
	pthread_cond_t init_cond;
	
	// Packet left from previous decode loop
	Packet *pkt;
	uint32_t inleft;
	uint8_t *inptr;
	
	double balance;
	double volume;
	bool muted;
	
	// input
	int stream_count;
	AudioStream *stream;
	IMediaDecoder *codec;
	
	// buffering
	uint8_t buffer[AUDIO_BUFLEN + 1];
	uint8_t *outbuf;
	uint8_t *outptr;
	
	// output
	snd_pcm_t *pcm;
	snd_pcm_uframes_t sample_size;
	struct pollfd *ufds;
	int nfds;
	
	// sync
	int64_t initial_pts;
	uint64_t pts_per_frame;
	
	Audio ();
	~Audio ();
};

Audio::Audio ()
{
	queue = new Queue ();
	
	pkt = NULL;
	inleft = 0;
	inptr = NULL;
	
	balance = 0.0f;
	volume = 1.0f;
	muted = false;
	
	stream_count = 0;
	stream = NULL;
	
	codec = NULL;
	
	memset (buffer, 0, AUDIO_BUFLEN + 1);
	outbuf = ALIGN (buffer, 2);
	outptr = outbuf;
	
	pcm = NULL;
	sample_size = 0;
	
	initial_pts = 0;
	pts_per_frame = 0;
	
	ufds = NULL;
	nfds = 0;
}

Audio::~Audio ()
{	
	g_free (ufds);
	ufds = NULL;

	if (pcm != NULL)
		snd_pcm_close (pcm);
	
	delete queue;
}

struct Video {
	Queue *queue;
	
	// input
	VideoStream *stream;
	IMediaDecoder *codec;
	
	// rendering
	cairo_surface_t *surface;
	uint8_t *rgb_buffer;
	
	// sync
	int64_t initial_pts;
	int msec_per_frame;
	double usec_to_pts;
	
	Video ();
	~Video ();
};

Video::Video ()
{
	queue = new Queue ();
	
	stream = NULL;
	codec = NULL;
	
	surface = NULL;
	rgb_buffer = NULL;
	
	initial_pts = 0;
	msec_per_frame = 0;
	usec_to_pts = 0;
}

Video::~Video ()
{
	delete queue;
}

MediaPlayer::MediaPlayer ()
{
	uri = NULL;
	asf_parser = NULL;
	
	pthread_mutex_init (&pause_mutex, NULL);
	pthread_cond_init (&pause_cond, NULL);
	pthread_mutex_lock (&pause_mutex);
	playing = false;
	opened = false;
	paused = true;
	stop = false;
	eof = false;
	
	av_ctx = NULL;
	
	audio_thread = NULL;
	io_thread = NULL;
	
	audio = new Audio ();
	video = new Video ();
	
	pause_time = 0;
	start_time = 0;
	
	pthread_mutex_init (&target_pts_lock, NULL);
	current_pts = 0;
	target_pts = 0;
	seek_pts = 0;
	
	height = 0;
	width = 0;
}

MediaPlayer::~MediaPlayer ()
{
	Close ();
	
	pthread_mutex_destroy (&target_pts_lock);
	
	pthread_mutex_unlock (&pause_mutex);
	pthread_mutex_destroy (&pause_mutex);
	pthread_cond_destroy (&pause_cond);
	
	g_free (uri);
	uri = NULL;
	
	delete audio;
	audio = NULL;
	delete video;
	video = NULL;
	delete media;
	media = NULL;
}

MediaResult
media_player_callback (MediaClosure* closure)
{
	//printf ("media_player_callback (%p). frame: %p, frame->uncompressed_size: %i\n", closure, closure->frame, closure->frame->uncompressed_size);
	
	MediaPlayer* player = (MediaPlayer*) closure->context;
	
	MediaFrame* frame = closure->frame;
	IMediaStream* stream = frame->stream;
	
	if (closure->frame == NULL || closure->frame->pts == 0)
		return MEDIA_SUCCESS;
	
	closure->frame = NULL;
	
	switch (stream->GetType ()) {
	case MediaTypeVideo: // TODO: Add locking here.
		player->video->queue->Push (new Packet (frame));
		return MEDIA_SUCCESS;
	case MediaTypeAudio: // TODO: Add locking here.
		printf ("Added audio packet, with pts: %lld\n", frame->pts);
		player->audio->queue->Push (new Packet (frame));
		return MEDIA_SUCCESS;
	default:
		return MEDIA_SUCCESS;
	}
}

bool
MediaPlayer::Open (const char *uri)
{
	IMediaDecoder *encoding;
	IMediaStream *stream;
	//int rv;
	
	Close ();
	
	g_free (this->uri);
	this->uri = NULL;
	
	printf ("MediaPlayer2::Open ('%s').\n", uri);
	
	if (uri == NULL || *uri == '\0')
		return false;

	MediaResult result;
	Media* media;
	
	media = new Media (NULL);
	result = media->Open (uri);
	if (!MEDIA_SUCCEEDED (result)) {
		fprintf (stderr, "MediaPlayer::Open ('%s'): cannot open uri: %i\n", uri, result);
		delete media;
		return false;
	}
	
	this->media = media;
	this->uri = g_strdup (uri);
	opened = true;
	
	// Set our frame reader callback
	MediaClosure* closure = new MediaClosure ();
	closure->media = media;
	closure->context = this;
	closure->callback = media_player_callback;
	media->SetQueueCallback (closure);
	
	// Find audio/video streams
	IMediaDemuxer* demuxer = media->GetDemuxer ();
	for (int i = 0; i < demuxer->GetStreamCount (); i++) {
		stream = demuxer->GetStream (i);
		encoding = stream->GetDecoder (); //stream->codec;
		
		if (encoding == NULL)
			continue; // No encoding was found for the stream.
		
		switch (stream->GetType ()) {
		case MediaTypeAudio:
			audio->stream_count++;
			
			audio->stream = (AudioStream*) stream;
			
			// starting time
			if (stream->start_time >= 0)
				audio->initial_pts = stream->start_time;
			else
				printf ("audio start pts is invalid? %lld\n", stream->start_time);
			printf ("audio initial_pts = %lld\n", audio->initial_pts);
			break;
		case MediaTypeVideo: 
			video->stream = (VideoStream*) stream;
			
			height = video->stream->height;
			width = video->stream->width;
			
			// for conversion to rgb32 format needed for rendering
			video->rgb_buffer = (uint8_t *) g_malloc0 (width * height * 4);
			/*video->scaler = sws_getContext (width, height, encoding->pixel_format,
							width, height, PIX_FMT_RGB32,
							SWS_BICUBIC, NULL, NULL, NULL);
			*/
			// rendering surface
			video->surface = cairo_image_surface_create_for_data (
				video->rgb_buffer, CAIRO_FORMAT_ARGB32,
				width, height, width * 4);
			
			// starting time
			if (stream->start_time >= 0)
				video->initial_pts = stream->start_time;
			else
				printf ("video start pts is invalid? %lld\n", stream->start_time);
			
			// msec per frame
			video->msec_per_frame = 33; //stream->msec_per_frame;
			
			// usec -> pts conversion
			video->usec_to_pts = 1.0 / 1000.0;// / stream->msec_per_frame); // TODO: is this equivalent?? //  (double) encoding->time_base.num / (double) encoding->time_base.den;
			//printf ("video initial_pts = %lld\n", video->initial_pts);
			
			//AVStream_dump (stream, 1);
			
			break;
		default:
			break;
		}
	}
	
	//AVFormatContext_dump (av_ctx);
	
	// Prepare audio playback
	if (!(moonlight_flags & RUNTIME_INIT_DISABLE_AUDIO) && audio->pcm == NULL && audio->stream!= NULL) {//_id != -1) {
 		if (snd_pcm_open (&audio->pcm, "default", SND_PCM_STREAM_PLAYBACK, 0) != 0) {
 			fprintf (stderr, "cannot open audio device: %s\n", strerror (errno));
 			audio->pcm = NULL;
 		}
	}
	
	if (audio->pcm != NULL && audio->stream != NULL) {//_id != -1) {
		snd_pcm_uframes_t buf_size;
		
		snd_pcm_set_params (audio->pcm, SND_PCM_FORMAT_S16,
				    SND_PCM_ACCESS_RW_INTERLEAVED,
				    audio->stream->channels,
				    audio->stream->sample_rate,
				    1, 0);
		
		snd_pcm_get_params (audio->pcm, &buf_size, &audio->sample_size);
		
		// 2 bytes per channel, we always calculate as 2-channel audio because it gets converted into such
		audio->pts_per_frame = (buf_size * 2 * 2) / (audio->stream->sample_rate / 100);
		
		target_pts = audio->initial_pts;
		printf ("initial pts (according to audio): %lld\n", target_pts);
	}
	
	if (video->stream != NULL) {//_id != -1) 
		media->GetNextFrameAsync (video->stream);
		media->GetNextFrameAsync (video->stream);
		media->GetNextFrameAsync (video->stream);
		media->GetNextFrameAsync (video->stream);
		media->GetNextFrameAsync (video->stream);
		media->GetNextFrameAsync (video->stream);
		media->GetNextFrameAsync (video->stream);
		media->GetNextFrameAsync (video->stream);
		
		target_pts = video->initial_pts;
		printf ("initial pts (according to video): %lld\n", target_pts);
	}
	
	return true;
}

void
MediaPlayer::Close ()
{
	Stop ();
	
	// Reset state back to what it was at instantiation
	
	if (video->rgb_buffer != NULL) {
		g_free (video->rgb_buffer);
		video->rgb_buffer = NULL;
	}
	
	if (video->surface != NULL) {
		cairo_surface_destroy (video->surface);
		video->surface = NULL;
	}
	
	delete media;
	media = NULL;
	
	playing = false;
	opened = false;
	eof = false;
	
	audio->stream_count = 0;
	audio->stream = NULL;
	audio->codec = NULL;
	audio->sample_size = 0;
	audio->initial_pts = 0;
	audio->pts_per_frame = 0;
	
	video->stream = NULL;
	video->codec = NULL;
	video->initial_pts = 0;
	video->msec_per_frame = 0;
	video->usec_to_pts = 0;
	
	pause_time = 0;
	start_time = 0;
	
	current_pts = 0;
	target_pts = 0;
	seek_pts = 0;
	
	height = 0;
	width = 0;
}

//
// Convert the AVframe into an RGB buffer we can render
//

static void
convert_to_rgb (Video *video, MediaFrame *frame)
{
	uint8_t *rgb_dest[3] = { video->rgb_buffer, NULL, NULL};
	int rgb_stride [3] = { video->stream->width * 4, 0, 0 };

	if (frame->uncompressed_data_stride == NULL || 
		frame->uncompressed_size <= 3 || 
		frame->uncompressed_data_stride[1] == NULL || 
		frame->uncompressed_data_stride[2] == NULL)
		return;
	
//	printf ("converting %p, %p, %p, %p, %p\n", frame->uncompressed_data_stride, frame->uncompressed_data_stride [0],
//	 frame->uncompressed_data_stride [1], frame->uncompressed_data_stride [2], frame->uncompressed_data_stride [3]);
	
	video->stream->converter->Convert (frame->uncompressed_data_stride, frame->srcStride, frame->srcSlideY, frame->srcSlideH, rgb_dest, rgb_stride);

}

bool
MediaPlayer::AdvanceFrame ()
{
	//printf ("MediaPlayer::AdvanceFrame ()\n");
	MediaFrame *frame = NULL;
	bool update = false;
	int64_t target_pts;
	Packet *pkt, *npkt;
	List *list;
	
	if (paused) {
		// shouldn't happen, but just in case
		printf ("WARNING: MediaPlayer::AdvanceFrame() called when paused\n");
		return false;
	}
	
	if (audio->stream == NULL) {
		// no audio to sync to
		uint64_t now = TimeManager::Instance()->GetCurrentTimeUsec();
		
		uint64_t elapsed_usec = now - start_time;
		uint64_t elapsed_pts = (uint64_t) (elapsed_usec * video->usec_to_pts);
		
		if (seek_pts == 0)
			target_pts = video->initial_pts + elapsed_pts;
		else
			target_pts = seek_pts + elapsed_pts;
		
		pthread_mutex_lock (&target_pts_lock);
		this->target_pts = target_pts;
		pthread_mutex_unlock (&target_pts_lock);
	} else {
	//	if (video->stream == NULL) {
	//		// No video, return false if we've reached the end of the audio or true otherwise
	//		return !MediaEnded ();
	//	}
	//	
	//	// use target_pts as set by audio thread
	//	pthread_mutex_lock (&target_pts_lock);
		target_pts = this->target_pts;
		//printf ("AdvanceFrame (), syncing to audio, target_pts: %lld\n", target_pts);
	//	pthread_mutex_unlock (&target_pts_lock);
	}
	
	if (current_pts >= seek_pts && current_pts >= target_pts) {
		printf ("MediaPlayer::AdvanceFrame () we're ahead of playback (current_pts = %lld, seek_pts = %lld, target_pts = %lld).\n", current_pts, seek_pts, target_pts);
		return !eof;
	}
	
	video->queue->Lock ();
	
	list = video->queue->LinkedList ();
	
	if (list->First () == NULL) {
		//printf ("MediaPlayer::AdvanceFrame (): nothing in the list.\n");
	}
	
	int count = list->Length ();
	int dropped = 0;
	
	if ((pkt = (Packet *) list->First ())) {
		do {
			// always decode the frame or we get glitches in the screen
			//frame = avcodec_alloc_frame ();
			
			//redraw = 0;
			//avcodec_decode_video (video->stream->codec, frame,
			//		      &redraw, pkt->data, pkt->size);
			
			update = true; //update || redraw;
			
			npkt = (Packet *) pkt->next;
			
			current_pts = pkt->pts;
			//printf ("MediaPlayer::AdvanceFrame (): got video frame with pts %lld, frame->pts = %lld, queue length: %i\n", pkt->pts, pkt->frame->pts, list->Length ());
			list->Unlink (pkt);
			
			//printf ("MediaPlayer::AdvanceFrame (): got video frame with pts %lld, frame->pts = %lld, queue length: %i\n", pkt->pts, pkt->frame->pts, list->Length ());
			
			if (current_pts >= target_pts) {
				// we are in sync (or ahead) of audio playback
				//printf ("MediaPlayer::AdvanceFrame () we are in sync (or ahead) of audio playback (current_pts = %lld, seek_pts = %lld, target_pts = %lld).\n", current_pts, seek_pts, target_pts);
				break;
			}
			
			if (!npkt) {
				// no more packets in queue, this frame is the most recent we have available
				//printf ("MediaPlayer::AdvanceFrame () no more packets in queue (current_pts = %lld, seek_pts = %lld, target_pts = %lld).\n", current_pts, seek_pts, target_pts);
				break;
			}
			
			delete pkt;
			media->GetNextFrameAsync (video->stream); // Request another frame
			dropped++;
			
			// we are lagging behind, drop this frame
			//printf ("MediaPlayer::AdvanceFrame () we are lagging behind, drop this frame (current_pts = %lld, seek_pts = %lld, target_pts = %lld).\n", current_pts, seek_pts, target_pts);
			frame = NULL;
			
			pkt = npkt;
		} while (pkt);
	}
	
	video->queue->Unlock ();
	
	if (update) {
		printf ("MediaPlayer::AdvanceFrame () (%.2i items in list, %.2i dropped) copying %i bytes to rgb buffer for current_pts = %lld (target_pts = %lld, pkt->pts = %lld), diff = %lld.\n", count, dropped, pkt->size, current_pts, target_pts, pkt->pts, target_pts - pkt->pts);
		convert_to_rgb (video, pkt->frame);
		//memcpy (video->rgb_buffer, pkt->data, pkt->size);
		media->GetNextFrameAsync (video->stream);
		if (count < 3) {
			media->GetNextFrameAsync (video->stream);
			printf ("Requested an extra frame.\n");
		}
		delete pkt;
		return true;
	}
	
	return !eof;
}

void
MediaPlayer::LoadVideoFrame ()
{
	bool update = false;
	MediaFrame *frame = NULL;
	
	if (video->stream == NULL)
		return;
		
	media->Seek (seek_pts);

	while ((frame = media->GetNextFrame (video->stream)) != NULL) {
		update = true;
		
		if (update && frame->pts >= (uint64_t) seek_pts) {
			convert_to_rgb (video, frame);
			delete frame;
			break;
		}
		delete frame;
		
	}
	
	media->Seek (seek_pts);
}

void
MediaPlayer::Render (cairo_t *cr)
{
	printf ("MediaPlayer::Render ()\n");
	if (video->stream == NULL) {//
		// no video to render
		return;
	}
	cairo_set_source_surface (cr, video->surface, 0, 0);
	cairo_rectangle (cr, 0, 0, width, height);
	cairo_fill (cr);
}

cairo_surface_t *
MediaPlayer::GetSurface ()
{
	return video->surface;
}

bool
MediaPlayer::IsPlaying ()
{
	return playing && !paused;
}

bool
MediaPlayer::MediaEnded ()
{
	if (!eof)
		return false;
	
	if ((audio->queue && !audio->queue->IsEmpty ()))
		return false;
	
	if ((video->queue && !video->queue->IsEmpty ()))
		return false;
	
	return true;
}

void
media_player_enqueue_frames (MediaPlayer* mp)
{
	Media* media = mp->media;
	
	if (mp->audio->stream != NULL) {
		for (int i = 0; i < 10; i++)
			media->GetNextFrameAsync (mp->audio->stream);
	}
	if (mp->video->stream != NULL) {
		media->GetNextFrameAsync (mp->video->stream);
	}
}

guint
MediaPlayer::Play (GSourceFunc callback, void *user_data)
{
	printf ("MediaPlayer::Play (), paused = %s, opened = %s, playing = %s\n", paused ? "true" : "false", opened ? "true" : "false", playing ? "true" : "false");
	if (!paused || !opened)
		return 0;
	
	if (!playing) {
		// Start up the decoder/audio threads
		media_player_enqueue_frames (this);
		
		if (audio->pcm != NULL && audio->stream != NULL) {
			pthread_mutex_lock (&audio->init_mutex);
			audio_thread = g_thread_create (audio_loop, this, true, NULL);
			pthread_cond_wait (&audio->init_cond, &audio->init_mutex);
			pthread_mutex_unlock (&audio->init_mutex);
		}
		
		playing = true;
	} else {
		// We are simply paused...
	}
	
	paused = false;
	pthread_cond_signal (&pause_cond);
	pthread_mutex_unlock (&pause_mutex);
	
	start_time += (TimeManager::Instance()->GetCurrentTimeUsec() - pause_time);
	
	if (video->stream != NULL) {
		printf ("MediaPlayer::Play (), timeout: %i\n", video->msec_per_frame);
		return TimeManager::Instance()->AddTimeout (video->msec_per_frame, callback, user_data);
	} else {
		printf ("MediaPlayer::Play (), timeout: 33 (no video)\n");
		return TimeManager::Instance()->AddTimeout (33, callback, user_data);
	}
	return 0;
}

bool
MediaPlayer::CanPause ()
{
	// FIXME: should return false if it is streaming media
	return true;
}

bool
MediaPlayer::IsPaused ()
{
	return paused;
}

void
MediaPlayer::Pause ()
{
printf ("MediaPlayer::Pause (), paused = %s, opened = %s, playing = %s\n", paused ? "true" : "false", opened ? "true" : "false", playing ? "true" : "false");
	if (paused || !CanPause ())
		return;
	
	paused = true;
	if (audio_thread != NULL)
		pthread_cond_wait (&pause_cond, &pause_mutex);
	else
		pthread_mutex_lock (&pause_mutex);
	
	pause_time = TimeManager::Instance()->GetCurrentTimeUsec();
}

void
MediaPlayer::StopThreads ()
{
	int64_t initial_pts;
	
	stop = true;
	
	if (paused) {
		paused = false;
		pthread_cond_signal (&pause_cond);
		pthread_mutex_unlock (&pause_mutex);
	}
	
	if (io_thread != NULL) {
		g_thread_join (io_thread);
		io_thread = NULL;
	}
	
	if (audio_thread != NULL) {
		g_thread_join (audio_thread);
		audio_thread = NULL;
	}
	
	// de-queue audio/video packets
	if (audio->pkt != NULL) {
		delete audio->pkt;
		audio->pkt = NULL;
		
		audio->inptr = NULL;
		audio->inleft = 0;
	}
	
	audio->queue->Clear (true);
	video->queue->Clear (true);
	
	// enter paused state
	pthread_mutex_lock (&pause_mutex);
	paused = true;
	
	audio->outptr = audio->outbuf;
	
	pause_time = 0;
	start_time = 0;
	
	//if (audio->pcm != NULL && audio->stream!= NULL)
	//	initial_pts = audio->initial_pts;
	//else
		initial_pts = video->initial_pts;
	
	current_pts = initial_pts;
	target_pts = initial_pts;
	seek_pts = 0;
	
	stop = false;
	eof = false;
}

void
MediaPlayer::Stop ()
{
	printf ("MediaPlayer::Stop (), paused = %s, opened = %s, playing = %s\n", paused ? "true" : "false", opened ? "true" : "false", playing ? "true" : "false");
	StopThreads ();
	
	playing = false;
	
	if (media != NULL)
		media->Seek (0);
}

bool
MediaPlayer::CanSeek ()
{
	// FIXME: should return false if it is streaming media
	return true;
}

void
MediaPlayer::Seek (int64_t position)
{
	//printf ("MediaPlayer::Seek (%lld), paused = %s, opened = %s, playing = %s\n", position, paused ? "true" : "false", opened ? "true" : "false", playing ? "true" : "false");
	int64_t initial_pts, duration;
	bool resume = !paused;
	//int stream_id;
	
	if (!CanSeek ())
		return;
	
	/*printf ("MediaPlayer::Seek (), audio-duration: %lld, audio-initialpts: %lld, video-duration: %lld, video-initial-pts: %lld\n",
		audio->stream != NULL ? audio->stream->duration : 0,
		audio->stream != NULL ? audio->initial_pts : 0,
		video->stream != NULL ? video->stream->duration : 0,
		video->stream != NULL ? video->initial_pts : 0);
		*/
	if (audio->pcm != NULL && audio->stream != NULL) {//_id != -1) {
		duration = audio->stream->duration;
		initial_pts = audio->initial_pts;
	} else {
		duration = video->stream->duration;
		initial_pts = video->initial_pts;
	}
	
	duration += initial_pts;
	position += initial_pts;
	
	if (position > duration)
		position = duration;
	else if (position < initial_pts)
		position = initial_pts;
	
	
	StopThreads ();
	media->DeleteQueue ();
	
	if (video->stream != NULL) {
		seek_pts = position;
		LoadVideoFrame ();
	} else {
		media->Seek (position);
		seek_pts = position;
	}
	
	printf ("MediaPlayer::Seek (%lld) seek_pts = %lld\n", position, seek_pts);
	
	if (playing) {
		media_player_enqueue_frames (this);

		// Restart the audio/io threads
		if (audio->pcm != NULL && audio->stream != NULL) {
			pthread_mutex_lock (&audio->init_mutex);
			audio_thread = g_thread_create (audio_loop, this, true, NULL);
			pthread_cond_wait (&audio->init_cond, &audio->init_mutex);
			pthread_mutex_unlock (&audio->init_mutex);
		}
		
		//io_thread = g_thread_create (io_loop, this, true, NULL);
		
		if (resume) {
			// Resume playback
			paused = false;
			pthread_cond_signal (&pause_cond);
			pthread_mutex_unlock (&pause_mutex);
			
			start_time = TimeManager::Instance()->GetCurrentTimeUsec();
		}
	}
}

int64_t
MediaPlayer::Position ()
{
	int64_t position = seek_pts > target_pts ? seek_pts : target_pts;
/*
	printf ("MediaPlayer::Position (), position: %lld, audio->initialpts: %lld, video->initialpts = %lld, seek_pts = %lld, target_pts = %lld, paused = %s, opened = %s, playing = %s\n", position,
		audio->initial_pts, video->initial_pts, seek_pts, target_pts, paused ? "true" : "false", opened ? "true" : "false", playing ? "true" : "false");
	*/	
	if (audio->pcm != NULL && audio->stream != NULL)
		return position - audio->initial_pts;
	
	if (video->stream != NULL)
		return position - video->initial_pts;
	
	return position;
}

int64_t
MediaPlayer::Duration ()
{
	printf ("MediaPlayer::Duration (), audio->stream->duration: %lld, video->stream->duration: %lld\n", audio->stream != NULL ? audio->stream->duration : -1,
			video->stream != NULL ? video->stream->duration : -1);
	
	if (audio->pcm != NULL && audio->stream != NULL)
		return audio->stream->duration;
	
	if (video->stream != NULL)
		return video->stream->duration;
	
	return 0;
}

void
MediaPlayer::Mute ()
{
	audio->muted = true;
}

void
MediaPlayer::UnMute ()
{
	audio->muted = false;
}

bool
MediaPlayer::IsMuted ()
{
	return audio->muted;
}

int
MediaPlayer::GetAudioStreamCount ()
{
	printf ("MediaPlayer::GetAudioStreamCount ().\n");
	return audio->stream_count;
}

bool
MediaPlayer::HasVideo ()
{
	return video->stream != NULL;
}

double
MediaPlayer::GetBalance ()
{
	return audio->balance;
}

void
MediaPlayer::SetBalance (double balance)
{
	if (balance < -1.0)
		balance = -1.0;
	else if (balance > 1.0)
		balance = 1.0;
	
	audio->balance = balance;
}

double
MediaPlayer::GetVolume ()
{
	return audio->volume;
}

void
MediaPlayer::SetVolume (double volume)
{
	if (volume < -1.0)
		volume = -1.0;
	else if (volume > 1.0)
		volume = 1.0;
	
	audio->volume = volume;
}

// Audio playback thread

//// Returns true if finished decoding, false otherwise
static bool
audio_decode (Audio *audio)
{
	int32_t frame_size = audio->sample_size * audio->stream->channels * 2;
	uint8_t *outbuf;
	int outlen = 0;
	int n;
	
	/* Note: an audio packet can contain multiple audio frames */
	
	while (audio->inleft > 0) {
		// TODO: Remove alignment
		outbuf = ALIGN (audio->outptr, 2);
		outlen = (audio->outbuf + AUDIO_BUFLEN) - outbuf;
		
		memcpy (outbuf, audio->inptr, audio->inleft);
		//printf ("audio_decode (), copied %i bytes.\n", audio->inleft);
		n = audio->inleft;
		
		if (n > 0) {
			audio->inleft -= n;
			audio->inptr += n;
			
			// append the newly decoded buffer to the end of the
			// remaining audio buffer from our previous loop
			// (if re-alignment was required).
			if (outbuf > audio->outptr) {
				//printf ("audio_decode (): memmove, outbuf: %p, outlen: %p\n", outbuf, outlen);
				memmove (audio->outptr, outbuf, outlen);
			}
			audio->outptr += n;
		} else if (n == 0) {
			/* complete or out of room in outbuf */
			break;
		} else {
			//printf ("audio_decode error: %d\n", n);
			// pad frame with silence
			if ((n = (audio->outptr - audio->outbuf) % frame_size) > 0) {
				memset (audio->outptr, 0, n);
				audio->outptr += n;
			}
			
			return true;
		}
	}
	
	return audio->inleft == 0;
}

static int
pcm_poll (snd_pcm_t *pcm, struct pollfd *ufds, int nfds)
{
	unsigned short revents;
	
	if (ufds == NULL)
		return 0;
	
	while (true) {
		poll (ufds, nfds, -1);
		snd_pcm_poll_descriptors_revents (pcm, ufds, nfds, &revents);
		if (revents & POLLERR)
			return -1;
		if (revents & POLLOUT)
			return 0;
	}
}

// Returns the frame pts or 0 if insufficient audio data
static uint64_t
audio_play (Audio *audio, bool play, struct pollfd *ufds, int nfds)
{
	int frame_size, samples, outlen, channels, n;
	uint8_t *outptr;
	
	channels = audio->stream->channels;
	frame_size = audio->sample_size * channels * 2;
	outlen = audio->outptr - audio->outbuf;
	samples = audio->sample_size;
	outptr = audio->outbuf;
	
	//printf ("audio_play (%p, %s, %p, %i) outlen: %i, frame_size: %i, samples: %i, channels: %i, muted: %s.\n", audio, play ? "true" : "false", ufds, nfds, 
	//	outlen, frame_size, samples, channels, audio->muted ? "true" : "false");
	
	// make sure we have enough data to play a frame of audio
	if (outlen < frame_size)
		return 0;
	
	if (!play) {
		outptr += samples * 2 * channels;
		goto finished;
	}
	
	if (!audio->muted) {
		//printf ("audio: volume: %f, balance: %f\n", audio->volume, audio->balance);
		// set balance/volume
		int16_t volume = (uint16_t) (audio->volume * 8192);
		int16_t *inptr = (int16_t *) audio->outbuf;
		int16_t leftvol, rightvol;
		int32_t vol;
		
		if (audio->balance < 0.0) {
			rightvol = (uint16_t) (1.0 + audio->balance) * volume;
			leftvol = volume;
		} else if (audio->balance > 0.0) {
			leftvol = (uint16_t) (1.0 - audio->balance) * volume;
			rightvol = volume;
		} else {
			leftvol = rightvol = volume;
		}
		
		for (n = 0; n < frame_size / 2; n += 2) {
			vol = (((int32_t) *inptr) * ((int32_t) leftvol)) >> 13;
			*inptr++ = (int16_t) CLAMP (vol, -32768, 32767);
			
			vol = (((int32_t) *inptr) * ((int32_t) rightvol)) >> 13;
			*inptr++ = (int16_t) CLAMP (vol, -32768, 32767);
		}
	} else {
		memset (audio->outbuf, 0, frame_size);
	}
	
	// play only 1 frame
	while (samples > 0) {
		//printf ("audio_play () samples: %i\n", samples);
		if (pcm_poll (audio->pcm, ufds, nfds) != 0) {
			switch (snd_pcm_state (audio->pcm)) {
			case SND_PCM_STATE_XRUN:
				printf ("SND_PCM_STATE_XRUN\n");
				snd_pcm_prepare (audio->pcm);
				break;
			case SND_PCM_STATE_SUSPENDED:
				printf ("SND_PCM_STATE_SUSPENDED\n");
				while ((n = snd_pcm_resume (audio->pcm)) == -EAGAIN)
					sleep (1);
				if (n < 0)
					snd_pcm_prepare (audio->pcm);
				break;
			default:
				break;
			}
		}
		
		//printf ("audio_play (): about to write...\n");
		n = snd_pcm_writei (audio->pcm, outptr, samples);
		/*printf ("audio_play (): wrote %i samples, n: %i, bytes: %i (1: %.8X, 2: %.8X, 3: %.8X, 4: %.8X) \n", samples, n, n * 2 * channels, 
			*(int*) outptr,
			*(1 + (int*) outptr),
			*(2 + (int*) outptr),
			*(3 + (int*) outptr));*/
		if (n > 0) {
			outptr += (n * 2 * channels);
			samples -= n;
		} else if (n == -ESTRPIPE) {
			printf ("snd_pcm_writei() returned -ESTRPIPE\n");
			while ((n = snd_pcm_resume (audio->pcm)) == -EAGAIN)
				sleep (1);
			
			if (n < 0) {
				snd_pcm_prepare (audio->pcm);
				snd_pcm_start (audio->pcm);
			}
		} else if (n == -EPIPE) {
			printf ("snd_pcm_writei() returned -EPIPE (underrun)\n");
			snd_pcm_prepare (audio->pcm);
			//snd_pcm_start (audio->pcm);
		}
	}
	
finished:
	
	if (outptr < audio->outptr) {
		// make room for more audio to be buffered
		outlen = audio->outptr - outptr;
		memmove (audio->outbuf, outptr, outlen);
		audio->outptr = audio->outbuf + outlen;
	} else {
		// no more buffered audio data
		audio->outptr = audio->outbuf;
	}
	
	return audio->pts_per_frame;
}

static void *
audio_loop (void *data)
{
	//printf ("audio_loop (): started.\n");
	MediaPlayer *mplayer = (MediaPlayer *) data;
	Audio *audio = mplayer->audio;
	struct pollfd *ufds = NULL;
	uint64_t frame_pts;
	Packet *pkt;
	bool play;
	int ndfs;
	
	pthread_mutex_lock (&mplayer->audio->init_mutex);
	
	if ((ndfs = snd_pcm_poll_descriptors_count (audio->pcm)) > 0) {
		ufds = (struct pollfd *) g_malloc0 (sizeof (struct pollfd) * ndfs);
		
		if (snd_pcm_poll_descriptors (audio->pcm, ufds, ndfs) < 0) {
			g_free (ufds);
			ufds = NULL;
		}
	}
	
	// signal the main thread that we are ready to begin audio playback
	pthread_cond_signal (&mplayer->audio->init_cond);
	pthread_mutex_unlock (&mplayer->audio->init_mutex);
	
	pthread_mutex_lock (&mplayer->pause_mutex);
	
	while (!mplayer->stop) {
		if (mplayer->paused) {
			printf ("audio_loop (): paused.\n");
			// allow main thread to take pause lock
			pthread_cond_signal (&mplayer->pause_cond);
			pthread_mutex_unlock (&mplayer->pause_mutex);
			
			// wait for main thread to relinquish pause lock
			pthread_cond_wait (&mplayer->pause_cond, &mplayer->pause_mutex);
			printf ("audio_loop (): resumed.\n");
			continue;
		}
		
		play = mplayer->target_pts >= mplayer->seek_pts;
		
		if ((frame_pts = audio_play (audio, play, ufds, ndfs)) > 0) {
			// calculated pts
			//printf ("frame_pts = %llu\n", frame_pts);
			pthread_mutex_lock (&mplayer->target_pts_lock);
			mplayer->target_pts += frame_pts;
			pthread_mutex_unlock (&mplayer->target_pts_lock);
			printf ("calculated target_pts = %llu (frame_pts: %lld)\n", mplayer->target_pts, frame_pts);
		} else {
			// decode an audio packet
			if (!audio->pkt && (pkt = (Packet *) audio->queue->Pop ())) {
				audio->inleft = pkt->size;
				audio->inptr = pkt->data;
				audio->pkt = pkt;
				
				pthread_mutex_lock (&mplayer->target_pts_lock);
				mplayer->target_pts = pkt->pts;
				pthread_mutex_unlock (&mplayer->target_pts_lock);
				printf ("setting target_pts to %llu\n", mplayer->target_pts);
				mplayer->media->GetNextFrameAsync (audio->stream);
				//printf ("audio_loop, popped a packet, %i packets left. inleft: %i, inptr: %p\n", audio->queue->Length (), audio->inleft, audio->inptr);
				
				if (!audio_decode (audio)) {
					printf ("audio_loop (), decode failed.\n");
				}
				delete audio->pkt;
				audio->pkt = NULL;
			}
			
		}
	}
	
	printf ("audio_loop (): exited.\n");
	
	pthread_mutex_unlock (&mplayer->pause_mutex);
	
	g_free (ufds);
	
	return NULL;
}

#endif // MOON_MEDIA
