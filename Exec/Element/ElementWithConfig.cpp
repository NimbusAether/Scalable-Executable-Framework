#include "ElementWithConfig.h"
#include "Config.h"


using SEF::Element::ElementWithConfig;
using namespace SEF::Config;

ElementWithConfig::ElementWithConfig(const string& elementName) 
{
	element_name = elementName;
	element_frequency = 0;
	element_enabled = false;
	start_frame = 0;
	type_name = "";
	element_mode = ModeType::Null;
	config_path = "";
}

ElementWithConfig::~ElementWithConfig() 
{

}

void ElementWithConfig::SetElementConfig(const string& elementName, int elementFrequency, bool elementEnabled, int startFrame, const string& typeName, const string& configFile)
{
	element_name = elementName;
	element_frequency = elementFrequency;
	element_enabled = elementEnabled;
	start_frame = startFrame;
	type_name = typeName;
	config_path = configFile;
	/*element_config->SetConfigFile(config_path, config_name);*/
}

void ElementWithConfig::SetElementConfig(int elementFrequency, bool elementEnabled, int startFrame, const string& typeName, const string& configFile)
{
	element_frequency = elementFrequency;
	element_enabled = elementEnabled;
	start_frame = startFrame;
	type_name = typeName;
	config_path = configFile;
}


void ElementWithConfig::SetElementConfigFile(const string& configFile)
{
	config_path = configFile;
}


