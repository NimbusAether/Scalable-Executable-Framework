#pragma once
#include "Element.h"
#include "ElementWithConfig.h"

namespace SEF {
	namespace Element {
		class UDM_EXPORT_API ElementFactory
		{
		public:
			ElementFactory();
			~ElementFactory();
			virtual Element* createElement(string typeName, string elementName);
			virtual ElementWithConfig* createElementWithConfig(string typeName, string elementName, string configureFile);
		};
	}
}
