#include "SemaphoreUdm.h"

using SEF::Udm::SemaphoreUdm;

SemaphoreUdm::SemaphoreUdm() 
{
	udm_type = UdmType::Semaphore;
}

SemaphoreUdm::~SemaphoreUdm() 
{

}

bool SemaphoreUdm::init()
{
	return false;
}

void SemaphoreUdm::read() 
{

}

void SemaphoreUdm::commit() 
{

}

bool SemaphoreUdm::publish(bool& output, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit)
{
	return false;
}

bool SemaphoreUdm::publish(int& output, const SEF_String& topicName, const  SEF_String& variableName, const  SEF_String& variableUnit) 
{
	return false;
}

bool SemaphoreUdm::publish(float& output, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit) 
{
	return false;
}

bool SemaphoreUdm::publish(double& output, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit)
{
	return false;
}

bool SemaphoreUdm::publish(SEF_String& output, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit)
{
	return false;
}

bool SemaphoreUdm::subscribe(bool& input, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit)
{
	return false;
}

bool SemaphoreUdm::subscribe(int& input, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit)
{
	return false;
}

bool SemaphoreUdm::subscribe(float& input, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit)
{
	return false;
}

bool SemaphoreUdm::subscribe(double& input, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit)
{
	return false;
}

bool SemaphoreUdm::subscribe(SEF_String& input, const SEF_String& topicName, const SEF_String& variableName,  const SEF_String& variableUnit)
{
	return false;
}

bool SEF::Udm::SemaphoreUdm::sendRequest(const ByteArray& bytes, const RequestQueryInfo& info)
{
	return false;
}


bool SEF::Udm::SemaphoreUdm::recvRequest(const RequestQueryInfo& info, RequestCb * cb)
{
	return false;
}
