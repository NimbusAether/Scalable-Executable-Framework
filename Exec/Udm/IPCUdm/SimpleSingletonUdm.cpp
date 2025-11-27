#include "SimpleSingletonUdm.h"

using SEF::Udm::SimpleSingletonUdm;

SimpleSingletonUdm::SimpleSingletonUdm() 
{
	udm_type = UdmType::SimpleSingleton;
}

SimpleSingletonUdm::~SimpleSingletonUdm() 
{

}

bool SimpleSingletonUdm::init()
{
	return false;
}

void SimpleSingletonUdm::read() 
{

}

void SimpleSingletonUdm::commit() 
{

}

bool SimpleSingletonUdm::publish(bool& output, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit) 
{
	return false;
}

bool SimpleSingletonUdm::publish(int& output, const SEF_String& topicName, const  SEF_String& variableName, const  SEF_String& variableUnit) 
{
	return false;
}

bool SimpleSingletonUdm::publish(float& output, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit) 
{
	return false;
}

bool SimpleSingletonUdm::publish(double& output, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit) 
{
	return false;
}

bool SimpleSingletonUdm::publish(SEF_String& output, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit) 
{
	return false;
}

bool SimpleSingletonUdm::subscribe(bool& input, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit)
{
	return false;
}

bool SimpleSingletonUdm::subscribe(int& input, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit)
{
	return false;
}

bool SimpleSingletonUdm::subscribe(float& input, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit)
{
	return false;
}

bool SimpleSingletonUdm::subscribe(double& input, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit)
{
	return false;
}

bool SimpleSingletonUdm::subscribe(SEF_String& input, const SEF_String& topicName, const SEF_String& variableName,  const SEF_String& variableUnit)
{
	return false;
}

bool SEF::Udm::SimpleSingletonUdm::sendRequest(const ByteArray& bytes, const RequestQueryInfo& info)
{
	return false;
}

bool SEF::Udm::SimpleSingletonUdm::recvRequest(const RequestQueryInfo& info, RequestCb * cb)
{
	return false;
}
