#include "ElementFactoryTest.h"
#include "Element1/Element1.h"


using SEF::Element::ElementFactoryTest;
using SEF::Element::ElementFactory;
using  SEF::Element::ElementWithConfig;
using  SEF::Element::Element;

void testFunc() {
	return;
}
ElementFactory* CreateFactory()
{
	return new ElementFactoryTest();
}

ElementFactoryTest::ElementFactoryTest() {

}

ElementFactoryTest::~ElementFactoryTest() {

}

Element* ElementFactoryTest::createElement(string typeName, string elementName) {
	Element* element = nullptr;

	if ("SEF::Elements::Element1::Element1" == typeName)
	{
		element = new SEF::Elements::Element1::Element1(elementName);
	}


	return element;
}
ElementWithConfig* ElementFactoryTest::createElementWithConfig(string typeName, string elementName, string configureFile) {
	ElementWithConfig* element = nullptr;

	if ("SEF::Elements::Element1::Element1" == typeName)
	{
		element = new SEF::Elements::Element1::Element1(elementName);
		element->SetElementConfig(100,true,1,"", configureFile);
	}
	return element;
}