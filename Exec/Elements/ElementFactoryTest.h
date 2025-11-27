#pragma once
#include "Element.h"
#include "ElementWithConfig.h"
#include "ElementFactory.h"
namespace SEF {
		namespace Element {
			class  ElementFactoryTest :public ElementFactory
			{
			public:
				ElementFactoryTest();
				~ElementFactoryTest();
				Element* createElement(string typeName, string elementName);
				ElementWithConfig* createElementWithConfig(string typeName, string elementName, string configureFile);
			};

			
		}
}

extern "C" UDM_EXPORT_API SEF::Element::ElementFactory* CreateFactory();
