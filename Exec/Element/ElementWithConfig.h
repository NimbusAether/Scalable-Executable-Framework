#pragma once
#include "Element.h"


namespace SEF {
	namespace Element {
		class UDM_EXPORT_API ElementWithConfig : public Element
		{
		public:
			ElementWithConfig(const string& elementName);
			virtual ~ElementWithConfig();
			void SetElementConfig(const string& elementName, int elementFrequency, bool elementEnabled, int startFrame, const string& typeName, const string& configFile);
			void SetElementConfig(int elementFrequency, bool elementEnabled, int startFrame, const string& typeName, const string& configFile);
			void SetElementConfigFile(const string& configFile);
		protected:
			string config_path;
		};
	}
}
