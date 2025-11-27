#include "Element.h"

using SEF::Element::Element;

Element::Element() 
{
	element_name = "";
	element_frequency = 0;
	element_enabled = false;
	start_frame = 0;
	type_name = "";
	element_mode = ModeType::Null;
}

Element::Element(const string& elementName) 
{
	element_name = elementName;
	element_frequency = 0;
	element_enabled = false;
	start_frame = 0;
	type_name = "";
	element_mode = ModeType::Null;
}

Element::~Element() 
{

}

void Element::SetElementConfig(const string& elementName, int elementFrequency, bool elementEnabled, int startFrame, const string& typeName)
{
	element_name = elementName;
	element_frequency = elementFrequency;
	element_enabled = elementEnabled;
	start_frame = startFrame;
	type_name = typeName;
}

void Element::SetElementConfig(int elementFrequency, bool elementEnabled, int startFrame, const string &typeName)
{
	element_frequency = elementFrequency;
	element_enabled = elementEnabled;
	start_frame = startFrame;
	type_name = typeName;
}

string Element::GetElementName() 
{
	return element_name;
}

int Element::GetElementFrequency() 
{
	return element_frequency;
}

string Element::GetTypeName() 
{
	return type_name;
}

ModeType Element::GetElementMode() 
{
	return element_mode;
}
bool Element::GetElementEnable()
{
	return element_enabled;
}
bool Element::SetElementMode(ModeType nextMode) 
{
	if (element_mode == ModeType::Startup && nextMode != ModeType::Initialize) 
	{
		return false;
	}
	else if (element_mode == ModeType::Shutdown) 
	{
		return false;
	}
	else 
	{
		if (nextMode == element_mode)
		{
			return true;
		}
		element_mode = nextMode;
		m_currentFrameIndex = 0;
		return true;
	}
}

void SEF::Element::Element::ModeFrameIncrease()
{
	m_currentFrameIndex++;
}

bool Element::Startup() 
{
	return false;
}

bool Element::PublishOutputs() 
{
	return false;
}

bool Element::SubscribeInputs() 
{
	return false;
}

bool Element::Initialize() 
{
	return false;
}

bool Element::Freeze() 
{
	return false;
}

bool Element::Run() 
{
	return false;
}

bool Element::Shutdown() 
{
	return false;
}

bool Element::isModeFirstFrame() const
{
	return m_currentFrameIndex == 0?true:false;
}