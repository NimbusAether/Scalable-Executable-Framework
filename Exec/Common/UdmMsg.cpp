#include "UdmMsg.h"
//#include "Udm.h"
#include <iostream>
#include "SafeQueue.h"
#include "UdmMsg.h"
#include "UdmRequestQueue.h"
#include <atomic>
#include <assert.h>
using SEF::Udm::UdmMsg;
using SEF::COMMON::ValueType;
using namespace SEF::COMMON;
using namespace SEF::Udm;
using namespace std;
#ifdef VLD_DEBUG
#include "vld.h"
#endif

std::atomic<unsigned long> counter(0);
UdmMsg::UdmMsg()
{
	m_type = ValueType::VALUE_TYPE_NONE;
	m_lock = LockStat::NONE;
	counter++;
	//std::cout << "\n#+++++++++++1+++++++++counter:" << counter << std::endl;

}

UdmMsg::UdmMsg(const UdmMsg& msg)
{
	m_type = msg.GetMsgType();
	m_sourceName = msg.GetSourceName();
	m_timestamp = msg.GetTimestamp();
	m_topicName = msg.GetTopicName();
	m_value = msg.GetValue();
	m_variableName = msg.GetVariableName();
	m_variableUnit = msg.GetVariableUnit();
	counter++;
	//std::cout << "\n#++++++++++++2++++++++counter:" << counter << std::endl;

}

UdmMsg::~UdmMsg() 
{
	counter--;
	//std::cout << "\n#---------------------counter:" << counter << std::endl;
	//if (counter > 1000)
	//{
	//	int i = 0;
	//}
}

bool UdmMsg::serialize(ValueType type, string& value, size_t valueSize, string& topicName, string& variableName, string& variableUnit)
{
	Request req;
	ByteArray reqBytes;
	req.setName("udmmessage");
	req.setSender(" ");
	req.add(topicName);
	req.add(variableName);
	req.add(variableUnit);
	req.add(m_sourceName);
	req.add(m_timestamp);
	req.add((SEF_Int32)type);
	req.add((SEF_Int32)m_lock);
	req.add(value);
	req.serialize(reqBytes);

	SEF_Size_t topicSize = topicName.size();
	SEF_Size_t  size = reqBytes.getSize() + topicSize + 2;
	SEF_Uchar* ptr = m_serializeBytes.reMalloc(size);
	memcpy(ptr, topicName.c_str(), topicSize);
	ptr[topicSize] = '@';
	ptr[topicSize + 1] = '\0';
	m_serializeBytes.setSize(topicSize+2);
	m_serializeBytes.appendBytes(reqBytes);
	return true;
}
bool UdmMsg::serialize(const ByteArray& bytes, ValueType& type, const string& topicName, const string& variableName, const string& variableUnit)
{
	Request req;
	ByteArray reqBytes;
	req.setName("udmmessage");
	req.setSender(" ");
	req.add(topicName);
	req.add(variableName);
	req.add(variableUnit);
	req.add(m_sourceName);
	req.add(m_timestamp);
	req.add((SEF_Int32)type);
	req.add((SEF_Int32)m_lock);
	req.add(bytes.getBytes(), reqBytes.getSize());
	req.serialize(reqBytes);

	SEF_Size_t topicSize = topicName.size();
	SEF_Size_t  size = reqBytes.getSize() + topicSize + 2;
	SEF_Uchar* ptr = m_serializeBytes.reMalloc(size);
	memcpy(ptr, topicName.c_str(), topicSize);
	ptr[topicSize] = '@';
	ptr[topicSize + 1] = '\0';
	m_serializeBytes.setSize(topicSize + 2);
	m_serializeBytes.appendBytes(reqBytes);
	return true;
}
const void* UdmMsg::GetBytes(SEF_Size_t& size)
{
	SEF_Size_t m_serializeSize = 0;
	SEF_Uchar* data = m_serializeBytes.getBytes(m_serializeSize);
	size = m_serializeSize;
	return data;
}
const ByteArray& UdmMsg::GetByteArray()const
{
	return m_serializeBytes;
}
bool SEF::Udm::UdmMsg::serialize()
{
	return serialize(m_value, m_type, m_topicName, m_variableName, m_variableUnit);
}
bool UdmMsg::deserialize(void* serializeBytes, size_t size)
{
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

	Request req;
	ByteArray array(ptr + parseSize, size - parseSize);
	req.deserialize(array);
	int paramCount = req.parameterCount();
	assert(paramCount == 7);

	int i = 0;
	string str = req.getString(i++);
	SetTopicName(str);

	str = req.getString(i++);
	SetVariableName(str);

	str = req.getString(i++);
	SetVariableUnit(str);

	str = req.getString(i++);
	SetSourceName(str);

	SetTimestamp(req.getUInt64(i++));

	SEF_Int32 type = req.getInt32(i++);
	SetMsgType((ValueType)type);

	SetLockStat(static_cast<LockStat>(req.getBool(i++)));

	ValueType paramType = req.getParameterType(i);
	if (paramType == ValueType::VALUE_TYPE_STRING) 
	{
		m_valueString = req.getString(i);
		m_value.setBytes(m_valueString.c_str(), m_valueString.size());
	}
	else if(paramType == ValueType::VALUE_TYPE_UCHARS)
	{
		m_value = req.getByteArray(i);
	}

	m_serializeBytes.setBytes(serializeBytes, size);
	return true;
}
ValueType UdmMsg::GetMsgType()const
{
	return m_type;
}
const ByteArray* UdmMsg::GetValue()const
{
	return &m_value;
}


const string &UdmMsg::GetValueString()const
{
	return m_valueString;
}

size_t UdmMsg::GetValueSize()const
{
	return m_valueSize;
}

string UdmMsg::GetTopicName()const
{
	return m_topicName;
}

string UdmMsg::GetVariableName()const
{
	return m_variableName;
}

string UdmMsg::GetVariableUnit()const
{
	return m_variableUnit;
}

string UdmMsg::GetSourceName()const
{
	return m_sourceName;
}

SEF_Uint64 UdmMsg::GetTimestamp()const
{
	return m_timestamp;
}

LockStat SEF::Udm::UdmMsg::GetLockStat() const
{
	return m_lock;
}

void UdmMsg::SetTopicName(const string &v)
{
	m_topicName = v;
}

void UdmMsg::SetVariableName(const string &v)
{
	m_variableName = v;
}

void UdmMsg::SetVariableUnit(const string &v)
{
	m_variableUnit = v;
}

void UdmMsg::SetMsgType(ValueType type)
{
	m_type = type;
}

void UdmMsg::SetSourceName(const string& sourceName)
{
	m_sourceName = sourceName;
}

void UdmMsg::SetTimestamp(SEF_Uint64 timestamp)
{
	m_timestamp = timestamp;
}

void SEF::Udm::UdmMsg::SetValue(const string& v)
{
	m_valueString = v;
}

void SEF::Udm::UdmMsg::SetValue(const ByteArray* v)
{
	m_value = v;
}

void SEF::Udm::UdmMsg::SetSerializeBytes(const ByteArray& v)
{
	m_serializeBytes = v;
}

void SEF::Udm::UdmMsg::SetLockStat(LockStat lockStat)
{
	m_lock = lockStat;
}


