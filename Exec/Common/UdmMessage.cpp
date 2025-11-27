//#include "Udm.h"
#include <iostream>
#include "SafeQueue.h"
#include "UdmMessage.h"
#include "json/json.h"
#include <atomic>
#include "udmMessage.pb.h"
using SEF::Udm::UdmMsg;
using SEF::Udm::UdmMessage;
using SEF::COMMON::ValueType;
using namespace SEF::COMMON;
using namespace SEF::Udm;
using namespace std;
using namespace UDM;
#ifdef VLD_DEBUG
#include "vld.h"
#endif
UdmMessage::UdmMessage()
{
	m_type = ValueType::VALUE_TYPE_NONE;
	m_message = new UDM::UDMMessage();
}

UdmMessage::UdmMessage(const UdmMsg& base)
{
	m_type = base.GetMsgType();
	m_sourceName = base.GetSourceName();
	m_timestamp = base.GetTimestamp();
	m_topicName = base.GetTopicName();
	m_value = base.GetValue();
	m_valueString = base.GetValueString();
	m_variableName = base.GetVariableName();
	m_variableUnit = base.GetVariableUnit();
	m_lock = base.GetLockStat();
	m_message = new UDM::UDMMessage();

	m_message->set_lockstat((int32_t)m_lock);
	m_message->set_message_type((int32_t)m_type);
	m_message->set_sourcename(m_sourceName);
	m_message->set_timestamp(m_timestamp);
	m_message->set_topicname(m_topicName);
	m_message->set_variablename(m_variableName);
	m_message->set_variableunit(m_variableUnit);
	m_serializeBytes = const_cast<ByteArray &>(base.GetByteArray());
}

UdmMessage::~UdmMessage() 
{
	if (m_message != nullptr) 
	{
		delete m_message;
		m_message = nullptr;
	}
}

bool SEF::Udm::UdmMessage::serialize()
{
	m_message->set_timestamp(m_timestamp);
	m_message->set_lockstat(static_cast<int32_t>(m_lock));

	m_message->set_timestamp(GetTimestamp());
	m_message->set_topicname(GetTopicName());
	m_message->set_variablename(GetVariableName());
	m_message->set_variableunit(GetVariableUnit());
	m_message->set_sourcename(GetSourceName());
	m_message->set_message_type(static_cast<int32_t>(GetMsgType()));
	if (m_value.getSize() > 0)
	{
		m_message->set_value(m_value.getBytes(), m_value.getSize());
	}
	else
	{
		m_message->set_value(GetValueString());
	}

	SEF_Size_t topicSize = m_message->topicname().size();
	SEF_Size_t HeaderSize = topicSize + 2;
	SEF_Size_t payloadSize = m_message->ByteSizeLong();

	SEF_Size_t size = payloadSize + HeaderSize;
	SEF_Uchar* ptr = m_serializeBytes.reMalloc(size);
	memcpy(ptr, m_message->topicname().c_str(), topicSize);
	ptr[topicSize] = '@';
	ptr[topicSize + 1] = '\0';

	bool res = m_message->SerializeToArray(ptr + HeaderSize, payloadSize);
	if (res != true)
	{
		throw "proto buffer serialize failed";
	}
	m_serializeBytes.setSize(size);
	return true;
}

bool UdmMessage::serialize(ValueType type, string & value, size_t valueSize, string& topicName, string& variableName, string& variableUnit)
{
	m_message->set_message_type((int32_t)type);
	m_message->set_value(value);
	m_message->set_variablename(variableName);
	m_message->set_variableunit(variableUnit);
	m_message->set_topicname(topicName);
	m_message->set_sourcename(m_sourceName);
	m_message->set_timestamp(m_timestamp);
	m_message->set_lockstat(static_cast<int32_t>(m_lock));
	SEF_Size_t topicSize = topicName.size();
	SEF_Size_t HeaderSize = topicSize + 2;
	SEF_Size_t payloadSize = m_message->ByteSizeLong();

	SEF_Size_t size = payloadSize + HeaderSize;
	SEF_Uchar* ptr = m_serializeBytes.reMalloc(size);
	memcpy(ptr, topicName.c_str(), topicSize);
	ptr[topicSize] = '@';
	ptr[topicSize + 1] = '\0';

	bool res = m_message->SerializeToArray(ptr+ HeaderSize, payloadSize);
	if (res != true) 
	{
		throw "proto buffer serialize failed";
	}

	m_serializeBytes.setSize(size);
	return true;
}
bool UdmMessage::serialize(const ByteArray& bytes, ValueType &type, const string& topicName, const string& variableName, const string& variableUnit)
{
	SEF_Size_t size = 0;
	COMMON::SEF_Uchar *data = bytes.getBytes(size);

	m_message->set_message_type((int32_t)type);
	m_message->set_value(data, size);
	m_message->set_variablename(variableName);
	m_message->set_variableunit(variableUnit);
	m_message->set_topicname(topicName);
	m_message->set_sourcename(m_sourceName);
	m_message->set_timestamp(m_timestamp);
	m_message->set_lockstat(static_cast<int32_t>(m_lock));

	SEF_Size_t topicSize = topicName.size();
	SEF_Size_t HeaderSize = topicSize + 2;
	SEF_Size_t payloadSize = m_message->ByteSizeLong();

	size = payloadSize + HeaderSize;
	SEF_Uchar* ptr = m_serializeBytes.reMalloc(size);
	memcpy(ptr, topicName.c_str(), topicSize);
	ptr[topicSize] = '@';
	ptr[topicSize+1] = '\0';

	bool res = m_message->SerializeToArray(ptr+ HeaderSize, payloadSize);
	if (res != true) 
	{
		throw "proto buffer serialize failed";
	}
	m_serializeBytes.setSize(size);
	return true;
}

bool UdmMessage::deserialize(void* serializeBytes, size_t size)
{
	bool res = false;
	int index = 0;
	size_t parseSize = 0;
	SEF_Char* ptr = (SEF_Char*)serializeBytes;
	SEF_Char* topicName = strtok(ptr, "@");
	if (topicName == nullptr) 
	{
		parseSize = 2;
	}
	else 
	{
		parseSize = strlen(topicName) + 2;
	}
	if (size < parseSize) 
	{
		return false;
	}

	res = m_message->ParseFromArray(ptr+ parseSize, size - parseSize);
	if (res == false) 
	{
		//throw "proto buffer parse failed";
		return false;
	}
	m_type = (ValueType)m_message->message_type();
	m_topicName = m_message->topicname();
	m_variableName = m_message->variablename();
	m_variableUnit = m_message->variableunit();
	m_valueSize = m_message->value().size();
	m_sourceName = m_message->sourcename();
	m_timestamp = m_message->timestamp();
	m_lock = static_cast<LockStat>(m_message->lockstat());
	//if (m_type == ValueType::VALUE_TYPE_REQUEST || m_type == ValueType::VALUE_TYPE_REQUESTQUEUE) {
	//	m_value.setBytes((const SEF_Uchar*)m_message->value().data(), (SEF_Size_t)m_valueSize);
	//}
	m_valueString = m_message->value();
	m_value.setBytes(m_message->value().data(), m_message->value().size());
	m_serializeBytes.setBytes(serializeBytes, size);
	return true;
}

UdmMessage& UdmMessage::operator=(UdmMessage& right)
{
	if (this != &right) 
	{
		m_type = right.m_type;
		m_valueSize = right.m_valueSize;
		m_variableName = right.m_variableName;
		m_topicName = right.m_topicName;
		m_variableUnit = right.m_variableUnit;
		m_value = right.m_value;
		m_valueString = right.m_valueString;
		m_sourceName = right.m_sourceName;
		m_lock = right.m_lock;
		m_message = new UDM::UDMMessage(*right.m_message);
		m_serializeBytes = right.m_serializeBytes;
	}
	return *this;
}

