/*
 * Automatically generated, do not edit this file directly
 */

#ifndef __MOONLIGHT_C_BINDING_H__
#define __MOONLIGHT_C_BINDING_H__

#include <glib.h>
#include <cairo.h>

#include "enums.h"

class Accessibility;
class AlsaSource;
class Animation;
class AnimationClock;
class Application;
class ArcSegment;
class ASFDemuxer;
class ASFMarkerDecoder;
class ASFPacket;
class ASFParser;
class AssemblyPart;
class AssemblyPartCollection;
class ASXDemuxer;
class AudioSource;
class AudioStream;
class BackEase;
class BeginStoryboard;
class BezierSegment;
class BitmapImage;
class BitmapSource;
class Border;
class BounceEase;
class Brush;
class Canvas;
class CircleEase;
class Clock;
class ClockGroup;
class CodecDownloader;
class Collection;
class CollectionChangedEventArgs;
class CollectionItemChangedEventArgs;
class ColorAnimation;
class ColorAnimationUsingKeyFrames;
class ColorKeyFrame;
class ColorKeyFrameCollection;
class ColumnDefinition;
class ColumnDefinitionCollection;
class ContentChangedEventArgs;
class ContentControl;
class Control;
class ControlTemplate;
class CubicEase;
class CursorPositionChangedEventArgs;
class DataTemplate;
class DeepZoomImageTileSource;
class DependencyObject;
class DependencyObjectCollection;
class Deployment;
class DiscreteColorKeyFrame;
class DiscreteDoubleKeyFrame;
class DiscreteObjectKeyFrame;
class DiscretePointKeyFrame;
class DispatcherTimer;
class DoubleCollection;
class DoubleAnimation;
class DoubleAnimationUsingKeyFrames;
class DoubleKeyFrame;
class DoubleKeyFrameCollection;
class Downloader;
class DownloadProgressEventArgs;
class DrawingAttributes;
class EasingColorKeyFrame;
class EasingDoubleKeyFrame;
class EasingFunctionBase;
class EasingPointKeyFrame;
class ElasticEase;
class Ellipse;
class EllipseGeometry;
class ErrorEventArgs;
class EventArgs;
class EventListenerProxy;
class EventObject;
class EventTrigger;
class ExceptionRoutedEventArgs;
class ExponentialEase;
class ExternalDecoder;
class ExternalDemuxer;
class FfmpegDecoder;
class FfmpegDemuxer;
class FileDownloader;
class FileSource;
class FrameworkElement;
class FrameworkTemplate;
class GeneralTransform;
class Geometry;
class GeometryCollection;
class GeometryGroup;
class Glyphs;
class GradientBrush;
class GradientStop;
class GradientStopCollection;
class Grid;
class HitTestCollection;
class IImageConverter;
class Image;
class ImageBrush;
class ImageErrorEventArgs;
class ImageSource;
class IMediaDecoder;
class IMediaDemuxer;
class IMediaObject;
class IMediaSource;
class IMediaStream;
class InkPresenter;
class Inline;
class InlineCollection;
class InputMethod;
class InternalDownloader;
class ItemCollection;
class KeyEventArgs;
class KeyFrame;
class KeyFrameCollection;
class KeySpline;
class LayoutInformation;
class Line;
class LinearColorKeyFrame;
class LinearDoubleKeyFrame;
class LinearGradientBrush;
class LinearPointKeyFrame;
class LineBreak;
class LineGeometry;
class LineSegment;
class ManagedStreamSource;
class ManualTimeSource;
class MarkerReachedEventArgs;
class MarkerStream;
class Matrix;
class MatrixTransform;
class Media;
class MediaAttribute;
class MediaAttributeCollection;
class MediaBase;
class MediaClosure;
class MediaDecodeFrameClosure;
class MediaElement;
class MediaFrame;
class MediaGetFrameClosure;
class MediaMarker;
class MediaMarkerFoundClosure;
class MediaPlayer;
class MediaReportSeekCompletedClosure;
class MediaSeekClosure;
class MemorySource;
class MmsDemuxer;
class MmsDownloader;
class MmsPlaylistEntry;
class MmsSecondDownloader;
class MmsSource;
class MouseEventArgs;
class MouseWheelEventArgs;
class Mp3Demuxer;
class MultiScaleImage;
class MultiScaleSubImage;
class MultiScaleSubImageCollection;
class MultiScaleTileSource;
class NameScope;
class NullDecoder;
class ObjectAnimationUsingKeyFrames;
class ObjectKeyFrame;
class ObjectKeyFrameCollection;
class Panel;
class ParallelTimeline;
class ParserErrorEventArgs;
class PassThroughDecoder;
class PasswordBox;
class Path;
class PathFigure;
class PathFigureCollection;
class PathGeometry;
class PathSegment;
class PathSegmentCollection;
class Playlist;
class PlaylistEntry;
class PlaylistRoot;
class PointCollection;
class PointAnimation;
class PointAnimationUsingKeyFrames;
class PointKeyFrame;
class PointKeyFrameCollection;
class PolyBezierSegment;
class Polygon;
class Polyline;
class PolyLineSegment;
class PolyQuadraticBezierSegment;
class Popup;
class PowerEase;
class ProgressEventArgs;
class ProgressiveSource;
class PulseSource;
class QuadraticBezierSegment;
class QuadraticEase;
class QuarticEase;
class QuinticEase;
class RadialGradientBrush;
class Rectangle;
class RectangleGeometry;
class RenderingEventArgs;
class ResourceDictionary;
class RotateTransform;
class RoutedEventArgs;
class RowDefinition;
class RowDefinitionCollection;
class Run;
class ScaleTransform;
class Setter;
class SetterBase;
class SetterBaseCollection;
class Shape;
class SineEase;
class SizeChangedEventArgs;
class SkewTransform;
class SolidColorBrush;
class SplineColorKeyFrame;
class SplineDoubleKeyFrame;
class SplinePointKeyFrame;
class Storyboard;
class Stroke;
class StrokeCollection;
class Style;
class StylusInfo;
class StylusPoint;
class StylusPointCollection;
class Surface;
class SystemTimeSource;
class TextBlock;
class TextBox;
class TextBoxBase;
class TextBoxModelChangedEventArgs;
class TextBoxView;
class TextChangedEventArgs;
class TileBrush;
class Timeline;
class TimelineCollection;
class TimelineGroup;
class TimelineMarker;
class TimelineMarkerCollection;
class TimeManager;
class TimeSource;
class Transform;
class TransformCollection;
class TransformGroup;
class TranslateTransform;
class TriggerCollection;
class TriggerAction;
class TriggerActionCollection;
class TriggerBase;
class UIElement;
class UIElementCollection;
class UnmanagedMatrix;
class UserControl;
class VideoBrush;
class VideoStream;
class VisualBrush;
class WriteableBitmap;
class YUVConverter;


class Accessibility;
class AlsaPlayer;
class AlsaSource;
class Animation;
class AnimationClock;
class AnimationStorage;
class Application;
class Applier;
class ArcSegment;
class ArrayList;
class ASFDemuxer;
class ASFDemuxerInfo;
class ASFFrameReader;
class ASFMarkerDecoder;
class ASFMarkerDecoderInfo;
class ASFPacket;
class ASFParser;
class ASFReader;
class AssemblyPart;
class AssemblyPartCollection;
class ASXDemuxer;
class ASXDemuxerInfo;
class AudioListNode;
class AudioPlayer;
class AudioSource;
class AudioSources;
class AudioStream;
class AutoCreatePropertyValueProvider;
class AutoCreators;
class BackEase;
class BeginStoryboard;
class BezierSegment;
class BitmapImage;
class BitmapSource;
class Border;
class BounceEase;
class BrowserBridge;
class Brush;
class Cancellable;
class Canvas;
class CircleEase;
class Clock;
class ClockGroup;
class CodecDownloader;
class Collection;
class CollectionChangedEventArgs;
class CollectionItemChangedEventArgs;
class CollectionIterator;
class ColorAnimation;
class ColorAnimationUsingKeyFrames;
class ColorKeyFrame;
class ColorKeyFrameCollection;
class ColumnDefinition;
class ColumnDefinitionCollection;
class ContentChangedEventArgs;
class ContentControl;
class ContentDescription;
class ContentDescriptionList;
class Control;
class ControlTemplate;
class ConverterInfo;
class CubicEase;
class CursorPositionChangedEventArgs;
class DataTemplate;
class DecoderInfo;
class DeepTreeWalker;
class DeepZoomImageTileSource;
class DefaultValuePropertyValueProvider;
class DemuxerInfo;
class DependencyObject;
class DependencyObjectCollection;
class DependencyProperty;
class Deployment;
class DeploymentStack;
class DirtyLists;
class DiscreteColorKeyFrame;
class DiscreteDoubleKeyFrame;
class DiscreteObjectKeyFrame;
class DiscretePointKeyFrame;
class DispatcherTimer;
class DoubleAnimation;
class DoubleAnimationUsingKeyFrames;
class DoubleCollection;
class DoubleKeyFrame;
class DoubleKeyFrameCollection;
class Downloader;
class DownloaderRequest;
class DownloaderResponse;
class DownloadProgressEventArgs;
class DrawingAttributes;
class EasingColorKeyFrame;
class EasingDoubleKeyFrame;
class EasingFunctionBase;
class EasingPointKeyFrame;
class ElasticEase;
class Ellipse;
class EllipseGeometry;
class ErrorEventArgs;
class EventArgs;
class EventListenerProxy;
class EventObject;
class EventTrigger;
class ExceptionRoutedEventArgs;
class ExponentialEase;
class ExternalDecoder;
class ExternalDecoderInfo;
class ExternalDemuxer;
class FfmpegDecoder;
class FfmpegDecoderInfo;
class FfmpegDemuxer;
class FileDownloader;
class FileSource;
class FontFace;
class FontManager;
class FrameworkElement;
class FrameworkElementProvider;
class FrameworkTemplate;
class GeneralTransform;
class Geometry;
class GeometryCollection;
class GeometryGroup;
class Glyphs;
class GradientBrush;
class GradientStop;
class GradientStopCollection;
class Grid;
class HitTestCollection;
class IDownloader;
class IImageConverter;
class Image;
class ImageBrush;
class ImageErrorEventArgs;
class ImageSource;
class IMediaDecoder;
class IMediaDemuxer;
class IMediaObject;
class IMediaSource;
class IMediaStream;
class InheritedPropertyValueProvider;
class InkPresenter;
class Inline;
class InlineCollection;
class InputMethod;
class InternalDownloader;
class ItemCollection;
class ITextAttributes;
class Keyboard;
class KeyEventArgs;
class KeyFrame;
class KeyFrameCollection;
class KeySpline;
class LayoutInformation;
class Line;
class LinearColorKeyFrame;
class LinearDoubleKeyFrame;
class LinearGradientBrush;
class LinearPointKeyFrame;
class LineBreak;
class LineGeometry;
class LineSegment;
class List;
class LocalPropertyValueProvider;
class ManagedStreamSource;
class ManualTimeSource;
class MarkerReachedEventArgs;
class MarkerStream;
class Matrix;
class MatrixTransform;
class Media;
class MediaAttribute;
class MediaAttributeCollection;
class MediaBase;
class MediaClosure;
class MediaDecodeFrameClosure;
class MediaElement;
class MediaElementPropertyValueProvider;
class MediaFrame;
class MediaGetFrameClosure;
class MediaInfo;
class MediaMarker;
class MediaMarkerFoundClosure;
class MediaPlayer;
class MediaReportSeekCompletedClosure;
class MediaSeekClosure;
class MediaWork;
class MemorySource;
class MmsDemuxer;
class MmsDownloader;
class MmsPlaylistEntry;
class MmsSecondDownloader;
class MmsSource;
class MoonError;
class MoonlightConfiguration;
class MoonWindow;
class MoonWindowGtk;
class MoonWindowless;
class MouseEventArgs;
class MouseWheelEventArgs;
class Mp3Demuxer;
class Mp3DemuxerInfo;
class Mp3FrameReader;
class MultiScaleImage;
class MultiScaleImagePropertyValueProvider;
class MultiScaleSubImage;
class MultiScaleSubImageCollection;
class MultiScaleTileSource;
class Mutex;
class NameScope;
class NPStreamRequest;
class NullDecoder;
class NullDecoderInfo;
class ObjectAnimationUsingKeyFrames;
class ObjectKeyFrame;
class ObjectKeyFrameCollection;
class ObjectTracker;
class Panel;
class ParallelTimeline;
class ParserErrorEventArgs;
class PassThroughDecoder;
class PassThroughDecoderInfo;
class PasswordBox;
class Path;
class PathFigure;
class PathFigureCollection;
class PathGeometry;
class PathSegment;
class PathSegmentCollection;
class Playlist;
class PlaylistEntry;
class PlaylistKind;
class PlaylistNode;
class PlaylistParser;
class PlaylistParserInternal;
class PlaylistRoot;
class PluginDownloader;
class PluginInstance;
class PluginXamlLoader;
class PointAnimation;
class PointAnimationUsingKeyFrames;
class PointCollection;
class PointKeyFrame;
class PointKeyFrameCollection;
class PolyBezierSegment;
class Polygon;
class Polyline;
class PolyLineSegment;
class PolyQuadraticBezierSegment;
class Popup;
class PowerEase;
class ProgressEventArgs;
class ProgressiveSource;
class PropertyValueProvider;
class PulsePlayer;
class PulseSource;
class QuadraticBezierSegment;
class QuadraticEase;
class QuarticEase;
class Queue;
class QuinticEase;
class RadialGradientBrush;
class Rectangle;
class RectangleGeometry;
class Region;
class RenderingEventArgs;
class RenderNode;
class ResourceDictionary;
class RotateTransform;
class RoutedEventArgs;
class RowDefinition;
class RowDefinitionCollection;
class Run;
class ScaleTransform;
class Setter;
class SetterBase;
class SetterBaseCollection;
class Shape;
class SineEase;
class SizeChangedEventArgs;
class SkewTransform;
class SolidColorBrush;
class SplineColorKeyFrame;
class SplineDoubleKeyFrame;
class SplinePointKeyFrame;
class Storyboard;
class StreamNotify;
class Stroke;
class StrokeCollection;
class Style;
class StylePropertyValueProvider;
class StylusInfo;
class StylusPoint;
class StylusPointCollection;
class Surface;
class SystemTimeSource;
class TabNavigationWalker;
class TextBlock;
class TextBox;
class TextBoxBase;
class TextBoxModelChangedEventArgs;
class TextBoxView;
class TextChangedEventArgs;
class TextFont;
class TextFontDescription;
class TextLayout;
class TextLayoutAttributes;
class TextStream;
class TileBrush;
class Timeline;
class TimelineCollection;
class TimelineGroup;
class TimelineMarker;
class TimelineMarkerCollection;
class TimeManager;
class TimeSource;
class ToggleNotifyListener;
class Transform;
class TransformCollection;
class TransformGroup;
class TranslateTransform;
class TriggerAction;
class TriggerActionCollection;
class TriggerBase;
class TriggerCollection;
class Type;
class Types;
class UIElement;
class UIElementCollection;
class UIElementNode;
class UnmanagedMatrix;
class UserControl;
class Validators;
class VideoBrush;
class VideoStream;
class VisualBrush;
class VisualTreeWalker;
class WriteableBitmap;
class XamlContext;
class XamlLoader;
class Xap;
class YUVConverter;
class YUVConverterInfo;

struct asf_bitrate_mutual_exclusion;
struct asf_codec_list;
struct asf_content_description;
struct asf_data;
struct asf_error_correction;
struct asf_error_correction_data;
struct asf_extended_content_description;
struct asf_extended_stream_name;
struct asf_extended_stream_properties;
struct asf_file_properties;
struct asf_guid;
struct asf_header;
struct asf_header_extension;
struct asf_marker;
struct asf_marker_entry;
struct asf_multiple_payloads;
struct asf_object;
struct asf_payload_extension_system;
struct asf_payload_parsing_information;
struct asf_script_command;
struct asf_script_command_entry;
struct asf_single_payload;
struct asf_stream_bitrate_properties;
struct asf_stream_properties;
struct asf_video_stream_data;
struct ASFContext;
struct ASFFrameReaderData;
struct ASFFrameReaderIndex;
struct AudioData;
struct BITMAPINFOHEADER;
struct Color;
struct CornerRadius;
struct Duration;
struct FontFaceExtents;
struct FontFamily;
struct FontSource;
struct FontStretch;
struct FontStyle;
struct FontWeight;
struct GlyphInfo;
struct GlyphMetrics;
struct GridLength;
struct KeyTime;
struct ManagedStreamCallbacks;
struct ManagedTypeInfo;
struct MmsDataPacket;
struct MmsHeader;
struct MmsHeaderReason;
struct MmsPacket;
struct MoonlightCollectionObject;
struct MoonlightCollectionType;
struct MoonlightContentObject;
struct MoonlightContentType;
struct MoonlightControlObject;
struct MoonlightControlType;
struct MoonlightCornerRadius;
struct MoonlightCornerRadiusType;
struct MoonlightDependencyObjectObject;
struct MoonlightDependencyObjectType;
struct MoonlightDownloaderObject;
struct MoonlightDownloaderType;
struct MoonlightDownloadProgressEventArgs;
struct MoonlightDownloadProgressEventArgsType;
struct MoonlightDuration;
struct MoonlightDurationType;
struct MoonlightErrorEventArgs;
struct MoonlightErrorEventArgsType;
struct MoonlightEventArgs;
struct MoonlightEventArgsType;
struct MoonlightEventObjectObject;
struct MoonlightEventObjectType;
struct MoonlightGridLength;
struct MoonlightGridLengthType;
struct MoonlightImageBrushObject;
struct MoonlightImageBrushType;
struct MoonlightImageObject;
struct MoonlightImageType;
struct MoonlightKeyEventArgsObject;
struct MoonlightKeyEventArgsType;
struct MoonlightKeyTime;
struct MoonlightKeyTimeType;
struct MoonlightMarkerReachedEventArgsObject;
struct MoonlightMarkerReachedEventArgsType;
struct MoonlightMediaElementObject;
struct MoonlightMediaElementType;
struct MoonlightMouseEventArgsObject;
struct MoonlightMouseEventArgsType;
struct MoonlightMultiScaleImageObject;
struct MoonlightMultiScaleImageType;
struct MoonlightObject;
struct MoonlightObjectType;
struct MoonlightPasswordBoxObject;
struct MoonlightPasswordBoxType;
struct MoonlightPoint;
struct MoonlightPointType;
struct MoonlightRect;
struct MoonlightRectType;
struct MoonlightRoutedEventArgs;
struct MoonlightRoutedEventArgsType;
struct MoonlightScriptableObjectObject;
struct MoonlightScriptableObjectType;
struct MoonlightScriptControlObject;
struct MoonlightScriptControlType;
struct MoonlightSettingsObject;
struct MoonlightSettingsType;
struct MoonlightStoryboardObject;
struct MoonlightStoryboardType;
struct MoonlightStrokeCollectionObject;
struct MoonlightStrokeCollectionType;
struct MoonlightStrokeObject;
struct MoonlightStrokeType;
struct MoonlightStylusInfoObject;
struct MoonlightStylusInfoType;
struct MoonlightStylusPointCollectionObject;
struct MoonlightStylusPointCollectionType;
struct MoonlightTextBlockObject;
struct MoonlightTextBlockType;
struct MoonlightTextBoxObject;
struct MoonlightTextBoxType;
struct MoonlightThickness;
struct MoonlightThicknessType;
struct MoonlightTimeSpan;
struct MoonlightTimeSpanType;
struct MoonlightUIElementObject;
struct MoonlightUIElementType;
struct MoonNameIdMapping;
struct MpegFrame;
struct MpegFrameHeader;
struct MpegVBRHeader;
struct Point;
struct PropertyChangedEventArgs;
struct PropertyPath;
struct Rect;
struct RepeatBehavior;
struct Size;
struct TextLayoutGlyphCluster;
struct TextLayoutLine;
struct TextLayoutRun;
struct Thickness;
struct Uri;
struct Value;
struct WAVEFORMATEX;
struct WAVEFORMATEXTENSIBLE;
struct XamlCallbackData;
struct XamlLoaderCallbacks;

typedef void ( * ApplyDefaultStyleCallback ) ( FrameworkElement * fwe , ManagedTypeInfo * key ) ;
typedef void ( * ApplyStyleCallback ) ( FrameworkElement * fwe , Style * style ) ;
typedef void * ( * ConvertKeyframeValueCallback ) ( int kind , DependencyProperty * property , Value * original , Value * converted ) ;
typedef ManagedStreamCallbacks ( * GetResourceCallback ) ( const char * name ) ;
typedef void ( * TickCallHandler ) ( EventObject * object ) ;
typedef void ( * EventHandler ) ( EventObject * sender , EventArgs * args , gpointer closure ) ;
typedef void ( * ToggleNotifyHandler ) ( EventObject * sender , bool isLastRef ) ;
typedef void ( * PropertyChangeHandler ) ( DependencyObject * sender , PropertyChangedEventArgs * args , MoonError * error , gpointer closure ) ;
typedef void ( * DownloaderResponseHeaderCallback ) ( gpointer context , const char * header , const char * value ) ;
typedef void ( * DownloaderWriteFunc ) ( void * buf , gint32 offset , gint32 n , gpointer cb_data ) ;
typedef void ( * DownloaderNotifySizeFunc ) ( gint64 size , gpointer cb_data ) ;
typedef gpointer ( * DownloaderCreateStateFunc ) ( Downloader * dl ) ;
typedef void ( * DownloaderDestroyStateFunc ) ( gpointer state ) ;
typedef void ( * DownloaderOpenFunc ) ( gpointer state , const char * verb , const char * uri , bool custom_header_support , bool disable_cache ) ;
typedef void ( * DownloaderSendFunc ) ( gpointer state ) ;
typedef void ( * DownloaderAbortFunc ) ( gpointer state ) ;
typedef void ( * DownloaderHeaderFunc ) ( gpointer state , const char * header , const char * value ) ;
typedef void ( * DownloaderBodyFunc ) ( gpointer state , void * body , guint32 length ) ;
typedef gpointer ( * DownloaderCreateWebRequestFunc ) ( const char * method , const char * uri , gpointer context ) ;
typedef void ( * DownloaderSetResponseHeaderCallbackFunc ) ( gpointer state , DownloaderResponseHeaderCallback callback , gpointer context ) ;
typedef DownloaderResponse * ( * DownloaderGetResponseFunc ) ( gpointer state ) ;
typedef guint32 ( * DownloaderResponseStartedHandler ) ( DownloaderResponse * response , gpointer context ) ;
typedef guint32 ( * DownloaderResponseDataAvailableHandler ) ( DownloaderResponse * response , gpointer context , char * buffer , guint32 length ) ;
typedef guint32 ( * DownloaderResponseFinishedHandler ) ( DownloaderResponse * response , gpointer context , bool success , gpointer data , const char * uri ) ;
typedef double ( * EasingFunction ) ( double normalizedTime ) ;
typedef Size ( * MeasureOverrideCallback ) ( Size availableSize ) ;
typedef Size ( * ArrangeOverrideCallback ) ( Size finalSize ) ;
typedef void ( * CloseDemuxerCallback ) ( void * instance ) ;
typedef void ( * GetDiagnosticAsyncCallback ) ( void * instance , int diagnosticKind ) ;
typedef void ( * GetFrameAsyncCallback ) ( void * instance , int mediaStreamType ) ;
typedef void ( * OpenDemuxerAsyncCallback ) ( void * instance , IMediaDemuxer * demuxer ) ;
typedef void ( * SeekAsyncCallback ) ( void * instance , guint64 seekToTime ) ;
typedef void ( * SwitchMediaStreamAsyncCallback ) ( void * instance , IMediaStream * mediaStreamDescription ) ;
typedef void ( * ExternalDecoder_DecodeFrameAsyncCallback ) ( void * instance , MediaFrame * frame ) ;
typedef void ( * ExternalDecoder_OpenDecoderAsyncCallback ) ( void * instance ) ;
typedef void ( * ExternalDecoder_CleanupCallback ) ( void * instance , MediaFrame * frame ) ;
typedef void ( * ExternalDecoder_CleanStateCallback ) ( void * instance ) ;
typedef bool ( * ExternalDecoder_HasDelayedFrameCallback ) ( void * instance ) ;
typedef void ( * ExternalDecoder_DisposeCallback ) ( void * instance ) ;
typedef void ( * ExternalDecoder_DtorCallback ) ( void * instance ) ;
typedef bool ( * ExternalDecoderInfo_SupportsCallback ) ( void * instance , const char * codec ) ;
typedef IMediaDecoder * ( * ExternalDecoderInfo_Create ) ( void * instance , Media * media , IMediaStream * stream ) ;
typedef void ( * ExternalDecoderInfo_dtor ) ( void * instance ) ;
typedef bool ( * get_image_uri_func ) ( int level , int posX , int posY , Uri * uri , void * user_data ) ;
typedef gint64 TimeSpan ;


G_BEGIN_DECLS

/**
 * PluginInstance
 **/
/* @GeneratePInvoke */
void *plugin_instance_evaluate (PluginInstance *instance, const char *code);

/* @GeneratePInvoke */
gint32 plugin_instance_get_actual_height (PluginInstance *instance);

/* @GeneratePInvoke */
gint32 plugin_instance_get_actual_width (PluginInstance *instance);

/* @GeneratePInvoke */
bool plugin_instance_get_allow_html_popup_window (PluginInstance *instance);

/* @GeneratePInvoke */
void *plugin_instance_get_browser_host (PluginInstance *instance);

/* @GeneratePInvoke */
bool plugin_instance_get_enable_html_access (PluginInstance *instance);

/* @GeneratePInvoke */
const char *plugin_instance_get_init_params (PluginInstance *instance);

/* @GeneratePInvoke */
const char *plugin_instance_get_source (PluginInstance *instance);

/* @GeneratePInvoke */
const char *plugin_instance_get_source_location (PluginInstance *instance);

/* @GeneratePInvoke */
const char *plugin_instance_get_source_location_original (PluginInstance *instance);

/* @GeneratePInvoke */
const char *plugin_instance_get_source_original (PluginInstance *instance);

/* @GeneratePInvoke */
Surface *plugin_instance_get_surface (PluginInstance *instance);

/* @GeneratePInvoke */
bool plugin_instance_get_windowless (PluginInstance *instance);

/* @GeneratePInvoke */
void plugin_instance_report_exception (PluginInstance *instance, char *msg, char *details, char* *stack_trace, int num_frames);


G_END_DECLS

#endif
