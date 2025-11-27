#include "ElementFactory.h"
//#include "Element1/Element1.h"
//#include "Element2/Element2.h"

using SEF::Element::ElementFactory;
using  SEF::Element::ElementWithConfig;
using  SEF::Element::Element;

ElementFactory::ElementFactory() 
{

}

ElementFactory::~ElementFactory()
{

}

Element* ElementFactory::createElement(string typeName, string elementName)
{
	Element* element = nullptr;

	//if ("SEF::Elements::Element1::Element1" == typeName)
	//{
	//	element = new SEF::Elements::Element1::Element1(elementName);
	//}
	//else if ("SEF::Elements::Element2::Element2" == typeName)
	//{
	//	element = new SEF::Elements::Element2::Element2(elementName);
	//}

	return element;
}
ElementWithConfig* ElementFactory::createElementWithConfig(string typeName, string elementName, string configureFile) 
{
	ElementWithConfig* element = nullptr;

	//if ("SEF::Elements::Element1::Element1" == typeName)
	//{
	//	element = new SEF::Elements::Element1::Element1(elementName);
	//	element->SetElementConfig(100,true,1,"", configureFile);
	//}
	//else if ("SEF::Elements::Element2::Element2" == typeName)
	//{
	//	element = new SEF::Elements::Element2::Element2(elementName);
	//	element->SetElementConfig(100, true, 1, "", configureFile);
	//}

	return element;
}