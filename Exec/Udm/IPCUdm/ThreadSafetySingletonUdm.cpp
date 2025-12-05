#include "ThreadSafetySingletonUdm.h"

using SEF::Udm::ThreadSafetySingletonUdm;

ThreadSafetySingletonUdm::ThreadSafetySingletonUdm() 
{
	udm_type = UdmType::ThreadSafetySingleton;
}

ThreadSafetySingletonUdm::~ThreadSafetySingletonUdm() 
{

}

bool ThreadSafetySingletonUdm::init()
{
	return false;
}

void ThreadSafetySingletonUdm::read() 
{

}

void ThreadSafetySingletonUdm::commit() 
{

}

bool ThreadSafetySingletonUdm::publish(bool& output, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit) 
{
	return false;
}

bool ThreadSafetySingletonUdm::publish(int& output, const SEF_String& topicName, const  SEF_String& variableName, const  SEF_String& variableUnit) 
{
	return false;
}

bool ThreadSafetySingletonUdm::publish(float& output, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit) 
{
	return false;
}

bool ThreadSafetySingletonUdm::publish(double& output, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit) 
{
	return false;
}

bool ThreadSafetySingletonUdm::publish(SEF_String& output, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit) 
{
	return false;
}

bool ThreadSafetySingletonUdm::subscribe(bool& input, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit)
{
	return false;
}

bool ThreadSafetySingletonUdm::subscribe(int& input, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit)
{
	return false;
}

bool ThreadSafetySingletonUdm::subscribe(float& input, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit)
{
	return false;
}

bool ThreadSafetySingletonUdm::subscribe(double& input, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit)
{
	return false;
}

bool ThreadSafetySingletonUdm::subscribe(SEF_String& input, const SEF_String& topicName, const SEF_String& variableName,  const SEF_String& variableUnit)
{
	return false;
}

bool SEF::Udm::ThreadSafetySingletonUdm::sendRequest(const ByteArray& bytes, const RequestQueryInfo& info)
{
	return false;
}

bool SEF::Udm::ThreadSafetySingletonUdm::recvRequest(const RequestQueryInfo& info, RequestCb * cb)
{
	return false;
}
