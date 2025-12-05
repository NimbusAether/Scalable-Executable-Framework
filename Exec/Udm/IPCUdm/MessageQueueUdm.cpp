#include "MessageQueueUdm.h"

using SEF::Udm::MessageQueueUdm;

MessageQueueUdm::MessageQueueUdm() 
{
	udm_type = UdmType::MessageQueue;
}

MessageQueueUdm::~MessageQueueUdm() 
{

}

bool MessageQueueUdm::init() 
{
	return false;
}

void MessageQueueUdm::read() 
{

}

void MessageQueueUdm::commit() 
{

}

bool MessageQueueUdm::publish(bool& output, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit) 
{
	return false;
}

bool MessageQueueUdm::publish(int& output, const SEF_String& topicName, const  SEF_String& variableName, const  SEF_String& variableUnit) 
{
	return false;
}

bool MessageQueueUdm::publish(float& output, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit) 
{
	return false;
}

bool MessageQueueUdm::publish(double& output, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit) 
{
	return false;
}

bool MessageQueueUdm::publish(SEF_String& output, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit) 
{
	return false;
}

bool MessageQueueUdm::subscribe(bool& input, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit)
{
	return false;
}

bool MessageQueueUdm::subscribe(int& input, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit)
{
	return false;
}

bool MessageQueueUdm::subscribe(float& input, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit)
{
	return false;
}

bool MessageQueueUdm::subscribe(double& input, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit)
{
	return false;
}

bool MessageQueueUdm::subscribe(SEF_String& input, const SEF_String& topicName, const SEF_String& variableName,  const SEF_String& variableUnit)
{
	return false;
}

bool SEF::Udm::MessageQueueUdm::sendRequest(const ByteArray& bytes, const RequestQueryInfo& info)
{
	return false;
}

bool SEF::Udm::MessageQueueUdm::recvRequest(const RequestQueryInfo& info, RequestCb *cb)
{
	return false;
}

