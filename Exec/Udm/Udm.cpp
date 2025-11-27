#include "Udm.h"
#include <iostream>
#include "log.h"
using SEF::Udm::Udm;
using namespace SEF::Udm;
using namespace SEF::Config;
using namespace SEF::Log;
Udm::Udm() 
{
	udm_name = "";
	udm_type = UdmType::None;
}

Udm::~Udm() 
{

}

SEF_String Udm::GetUdmName()
{
	return udm_name;
}

UdmType Udm::GetUdmType()
{
	return udm_type;
}

void Udm::log(const SEF_String &elementName, SEFLogLevel logType, const SEF_String &logMessage) 
{
	switch(logType) {
		case SEFLogLevel::Info:
			LOGI("[elementName:"+elementName+"]:"+logMessage);
			break;
		case SEFLogLevel::Warning:
			LOGW("[elementName:" + elementName + "]:" + logMessage);
			break;
		case SEFLogLevel::Error:
			LOGE("[elementName:" + elementName + "]:" + logMessage);
			break;
		case SEFLogLevel::Fatal:
			LOGF("[elementName:" + elementName + "]:" + logMessage);
			break;
		case SEFLogLevel::Debug:
			LOGD("[elementName:" + elementName + "]:" + logMessage);
			break;
		case SEFLogLevel::Alarm:
			LOGA("[elementName:" + elementName + "]:" + logMessage);
			break;
	}	
}

void Udm::SetUdmName(const SEF_String& udmName)
{
	udm_name = udmName;
}

void Udm::appendUdmAddress(const UdmAddress& info)
{
	udm_address.push_back(info);
}
void SEF::Udm::Udm::SetConfigureFile(const SEF_String& file)
{
	udm_configFile = file;
}
