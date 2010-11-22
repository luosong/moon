// Copyright (C) 2009 Novell, Inc (http://www.novell.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Mono.Cecil;

namespace Moonlight.SecurityModel {

	static public class CecilRocks {

		private const string SecurityCritical = "System.Security.SecurityCriticalAttribute";
		private const string SecuritySafeCritical = "System.Security.SecuritySafeCriticalAttribute";

		public static bool HasAttribute (this ICustomAttributeProvider self, string attribute)
		{
			if (!self.HasCustomAttributes)
				return false;

			foreach (CustomAttribute ca in self.CustomAttributes) {
				if (ca.AttributeType.FullName == attribute)
					return true;
			}
			return false;
		}

		public static bool IsSecurityCritical (this MethodDefinition self)
		{
			if (self.HasCustomAttributes && HasAttribute (self, SecurityCritical))
				return true;
			return IsSecurityCritical (self.DeclaringType);
		}

		public static bool IsSecurityCritical (this TypeDefinition self)
		{
			bool result = false;
			if (self.HasCustomAttributes)
				result = HasAttribute (self, SecurityCritical);

			if (result)
				return true;
			else if (self.IsNested)
				return self.DeclaringType.IsSecurityCritical ();
			else
				return false;
		}

		public static bool IsSecuritySafeCritical (this MethodDefinition self)
		{
			if (self.HasCustomAttributes && HasAttribute (self, SecuritySafeCritical))
				return true;
			return IsSecuritySafeCritical (self.DeclaringType);
		}

		public static bool IsSecuritySafeCritical (this TypeDefinition self)
		{
			bool result = false;
			if (self.HasCustomAttributes)
				result = HasAttribute (self, SecuritySafeCritical);

			if (result)
				return true;
			else if (self.IsNested)
				return self.DeclaringType.IsSecuritySafeCritical ();
			else
				return false;
		}

		public static bool IsVisible (this TypeDefinition self)
		{
			while (self.IsNested) {
				if (self.IsNestedPrivate || self.IsNestedAssembly)
					return false;
				// Nested classes are always inside the same assembly, so the cast is ok
				self = (self.DeclaringType as TypeDefinition);
			}
			return self.IsPublic;
		}

		public static bool IsVisible (this MethodDefinition self)
		{
			if (self.IsPrivate || self.IsAssembly)
				return false;
			return (self.DeclaringType as TypeDefinition).IsVisible ();
		}

		public static bool IsVisible (this FieldDefinition self)
		{
			if (self.IsPrivate || self.IsAssembly)
				return false;
			return (self.DeclaringType as TypeDefinition).IsVisible ();
		}

		public static string GetFullName (this MethodReference self)
		{
			StringBuilder sb = new StringBuilder ();
			sb.Append (self.ReturnType.FullName);
			sb.Append (" ");
			sb.Append (self.DeclaringType.FullName);
			sb.Append ("::");
			sb.Append (self.Name);
			if (self.HasGenericParameters) {
				sb.Append ("<");
				for (int i = 0; i < self.GenericParameters.Count; i++ ) {
					if (i > 0)
						sb.Append (",");
					sb.Append (self.GenericParameters [i].Name);
				}
				sb.Append (">");
			}
			sb.Append ("(");
			if (self.HasParameters) {
				for (int i = 0; i < self.Parameters.Count; i++) {
					if (i > 0)
						sb.Append (",");

					if (self.Parameters [i].ParameterType is SentinelType)
						sb.Append ("...,");

					sb.Append (self.Parameters [i].ParameterType.FullName);
				}
			}
			sb.Append (")");
			return sb.ToString ();
		}

		public static IEnumerable<TypeDefinition> GetAllTypes (this ModuleDefinition self)
		{
			return self.Types.SelectMany (t => t.GetAllTypes ());
		}

		static IEnumerable<TypeDefinition> GetAllTypes (this TypeDefinition self)
		{
			yield return self;

			if (!self.HasNestedTypes)
				yield break;

			foreach (var type in self.NestedTypes.SelectMany (t => t.GetAllTypes ()))
				yield return type;
		}

		public static IEnumerable<MethodDefinition> GetMethods (this TypeDefinition self)
		{
			return self.Methods.Where (m => !m.IsConstructor);
		}

		public static IEnumerable<MethodDefinition> GetConstructors (this TypeDefinition self)
		{
			return self.Methods.Where (m => m.IsConstructor);
		}
	}
}

