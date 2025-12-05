#include "SharedMemoryUdm.h"

using SEF::Udm::SharedMemoryUdm;

SharedMemoryUdm::SharedMemoryUdm() 
{
	udm_type = UdmType::SharedMemory;
}

SharedMemoryUdm::~SharedMemoryUdm() 
{

}

bool SharedMemoryUdm::init()
{
	return false;
}

void SharedMemoryUdm::read() 
{

}

void SharedMemoryUdm::commit() 
{

}

bool SharedMemoryUdm::publish(bool& output, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit) 
{
	return false;
}

bool SharedMemoryUdm::publish(int& output, const SEF_String& topicName, const  SEF_String& variableName, const  SEF_String& variableUnit) 
{
	return false;
}

bool SharedMemoryUdm::publish(float& output, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit) 
{
	return false;
}

bool SharedMemoryUdm::publish(double& output, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit) 
{
	return false;
}

bool SharedMemoryUdm::publish(SEF_String& output, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit) 
{
	return false;
}

bool SharedMemoryUdm::subscribe(bool& input, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit)
{
	return false;
}

bool SharedMemoryUdm::subscribe(int& input, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit)
{
	return false;
}

bool SharedMemoryUdm::subscribe(float& input, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit)
{
	return false;
}

bool SharedMemoryUdm::subscribe(double& input, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit)
{
	return false;
}

bool SharedMemoryUdm::subscribe(SEF_String& input, const SEF_String& topicName, const SEF_String& variableName,  const SEF_String& variableUnit)
{
	return false;
}

bool SEF::Udm::SharedMemoryUdm::sendRequest(const ByteArray& bytes, const RequestQueryInfo& info)
{
	return false;
}


bool SEF::Udm::SharedMemoryUdm::recvRequest(const RequestQueryInfo& info, RequestCb * cb)
{
	return false;
}
