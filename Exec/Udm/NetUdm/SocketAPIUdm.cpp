#include "SocketAPIUdm.h"

using SEF::Udm::SocketAPIUdm;

SocketAPIUdm::SocketAPIUdm() 
{
	udm_type = UdmType::SocketAPI;
}

SocketAPIUdm::~SocketAPIUdm() 
{

}

bool SocketAPIUdm::init() 
{
	return false;
}

void SocketAPIUdm::read() 
{

}

void SocketAPIUdm::commit()
{

}

bool SocketAPIUdm::publish(bool& output, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit) 
{
	return false;
}

bool SocketAPIUdm::publish(int& output, const SEF_String& topicName, const  SEF_String& variableName, const  SEF_String& variableUnit) 
{
	return false;
}

bool SocketAPIUdm::publish(float& output, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit) 
{
	return false;
}

bool SocketAPIUdm::publish(double& output, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit) 
{
	return false;
}

bool SocketAPIUdm::publish(SEF_String& output, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit)
{
	return false;
}

bool SocketAPIUdm::subscribe(bool& input, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit)
{
	return false;
}

bool SocketAPIUdm::subscribe(int& input, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit)
{
	return false;
}

bool SocketAPIUdm::subscribe(float& input, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit)
{
	return false;
}

bool SocketAPIUdm::subscribe(double& input, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit)
{
	return false;
}

bool SocketAPIUdm::subscribe(SEF_String& input, const SEF_String& topicName, const SEF_String& variableName,  const SEF_String& variableUnit)
{
	return false;
}

bool SocketAPIUdm::subscribe(NotifyCb* cb, const VarQueryInfo& index)
{
	return false;
}

bool SocketAPIUdm::sendRequest(const ByteArray& bytes, const RequestQueryInfo& info)
{

	return false;
}

bool SocketAPIUdm::recvRequest(const RequestQueryInfo& info, RequestCb * cb)
{
	return false;
}
