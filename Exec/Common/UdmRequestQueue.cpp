#include "UdmRequestQueue.h"
#include"platform.h"
#include <assert.h>
#include <iostream>

#ifdef VLD_DEBUG
#include "vld.h"
#endif
using namespace SEF::COMMON;
using namespace std;

std::atomic<unsigned long> paramCounter(0);
std::atomic<unsigned long> requestCounter(0);

typedef struct ParamaterHeader {
	unsigned char type;
	unsigned char length[4];
}ParamaterHeadeT;
/*
*	version: 5
*	marker: 1标识为最后一个消息
*	ext：扩展标识位
*	type: request为 1 requestQueue为 2
*/
typedef struct baseHeader {
#if defined(BIG_ENDIAN)
	unsigned char version : 3;
	unsigned char marker : 1;
	unsigned char ext : 1;
	unsigned char type : 3;
#elif defined(LITTLE_ENDIAN)
	unsigned char type : 3;
	unsigned char ext : 1;
	unsigned char marker : 1;
	unsigned char version : 3;
#else
#error "YOU MUST DEFINE BYTE ORDER LITTLE_ENDIAN OR BIG_ENDIAN !"
#endif
}baseHeader;



/*
*  base_header|cseq|length|parameter_count|requestNameSize|senderNameSize|requestName|senderName|paramater data
*/
typedef struct RequestHeader {
	baseHeader		base_header;
	unsigned char	cseq[2];
	unsigned char	length[4];
	unsigned char	parameter_count[4];
	unsigned char	requestNameSize[2];
	unsigned char	senderNameSize[2];
}RequestHeaderT;

typedef struct RequestQueueHeader {
	baseHeader		base_header;
	unsigned char queueNameSize[4];
	unsigned char length[4];
}RequestQueueHeaderT;


class RequestHead {
public:
	RequestHead() 
	{

	}
	~RequestHead() 
	{

	}
private:
	ByteArray		m_bytes;
	SEF_Uint16		m_cseq;
	SEF_Uint32		m_length;
	SEF_Uint32		m_parameterCount;
	SEF_Uint16		m_requestNameSize;
	SEF_Uint16		m_senderNameSize;
};

constexpr SEF_Size_t paramHeaderSize = sizeof(ParamaterHeadeT);
constexpr SEF_Size_t requestHeaderSize = sizeof(RequestHeaderT);
constexpr SEF_Size_t queueHeaderSize = sizeof(RequestQueueHeaderT);
constexpr int gVersion = 3;


static inline  void baseHeaderInit(baseHeader& header, int version, int marker, int ext, int type)
{
	header.ext = ext;
	header.marker = marker;
	header.type = type;
	header.version = version; 
}

/*
* 	unsigned char version : 3;
	unsigned char marker : 1;
	unsigned char ext : 1;
	unsigned char type : 3;
*/
static inline  void baseHeaderParser(SEF_Uchar buf, baseHeader &header)
{
	header.version = buf &0x07;
	header.marker = (buf >> 3)&0x01;
	header.ext = (buf >> 4) & 0x01;
	header.type = (buf>>5)& 0x03;
}

void SEF::COMMON::Parameter::CountAdd()
{
	paramCounter++;
}

void SEF::COMMON::Parameter::CountDel()
{
	paramCounter--;
	if (paramCounter > 1000&& paramCounter%1000==1)
	{
		std::cout << "\n#+++++++++++1+++++++++paramCounter:" << paramCounter << std::endl;
	}
}

Parameter::Parameter(const Parameter& right)
{
	m_type = right.m_type;
	m_bytes = right.m_bytes;
	CountAdd();
}

Parameter::Parameter(const ValueType& type)
{
	m_type = type;
	CountAdd();
}

Parameter::Parameter(const ValueType& type, const ByteArray &bytes)
{
	m_type = type;
	m_bytes = bytes;
	CountAdd();
}

Parameter::Parameter(const SEF_String& arg)
{
	m_type = ValueType::VALUE_TYPE_STRING;
	//m_bytes.reMalloc(arg.size()+1);
	m_bytes.setBytes((const SEF_Uchar*)arg.c_str(), arg.size()+1);
	CountAdd();
}

Parameter::Parameter(const SEF_Bool& arg)
{
	m_type = ValueType::VALUE_TYPE_BOOL;
	m_bytes.setBytes((const SEF_Uchar*)&arg, sizeof(SEF_Bool));
	CountAdd();
}

Parameter::Parameter(const SEF_Double& arg)
{
	m_type = ValueType::VALUE_TYPE_DOUBLE;
	m_bytes.setBytes((const SEF_Uchar*)&arg, sizeof(SEF_Double));
	CountAdd();
}

Parameter::Parameter(const SEF_Float& arg)
{
	m_type = ValueType::VALUE_TYPE_FLOAT;
	m_bytes.setBytes((const SEF_Uchar*)&arg, sizeof(SEF_Float));
	CountAdd();
}

Parameter::Parameter(const SEF_Long& arg)
{
	m_type = ValueType::VALUE_TYPE_LONG;
	m_bytes.setBytes((const SEF_Uchar*)&arg, sizeof(SEF_Long));
	CountAdd();
}

Parameter::Parameter(const SEF_Int64& arg)
{
	m_type = ValueType::VALUE_TYPE_INT64;
	m_bytes.setBytes((const SEF_Uchar*)&arg, sizeof(SEF_Int64));
	CountAdd();
}
Parameter::Parameter(const SEF_Uint64& arg)
{
	m_type = ValueType::VALUE_TYPE_UINT64;
	m_bytes.setBytes((const SEF_Uchar*)&arg, sizeof(SEF_Uint64));
	CountAdd();
}

Parameter::Parameter(const SEF_Uint32& arg)
{
	m_type = ValueType::VALUE_TYPE_UINT32;
	m_bytes.setBytes((const SEF_Uchar*)&arg, sizeof(SEF_Uint32));
	CountAdd();
}
Parameter::Parameter(const SEF_Int32& arg)
{
	m_type = ValueType::VALUE_TYPE_INT32;
	m_bytes.setBytes((const SEF_Uchar*)&arg, sizeof(SEF_Int32));
	CountAdd();
}

Parameter::Parameter(const SEF_Uint16& arg)
{
	m_type = ValueType::VALUE_TYPE_UINT16;
	m_bytes.setBytes((const SEF_Uchar*)&arg, sizeof(SEF_Uint16));
	CountAdd();
}

Parameter::Parameter(const SEF_Int16& arg)
{
	m_type = ValueType::VALUE_TYPE_INT16;
	m_bytes.setBytes((const SEF_Uchar*)&arg, sizeof(SEF_Int16));
	CountAdd();
}

Parameter::Parameter(const SEF_Uchar& arg)
{
	m_type = ValueType::VALUE_TYPE_UCHAR;
	m_bytes.setBytes((const SEF_Uchar*)&arg, sizeof(SEF_Uchar));
	CountAdd();
}

Parameter::Parameter(const SEF_Char& arg)
{
	m_type = ValueType::VALUE_TYPE_CHAR;
	m_bytes.setBytes((const SEF_Uchar*)&arg, sizeof(SEF_Char));
	CountAdd();
}

Parameter::Parameter(const COMMON::SEF_Uchar* arg, SEF_Size_t size)
{
	m_type = ValueType::VALUE_TYPE_UCHARS;
	m_bytes.setBytes((const SEF_Uchar*)&arg, size);
	CountAdd();
}

Parameter::Parameter(const COMMON::SEF_Char* arg, COMMON::SEF_Size_t size)
{
	m_type = ValueType::VALUE_TYPE_STRING;
	//m_bytes.reMalloc(arg.size()+1);
	m_bytes.setBytes((const SEF_Uchar*)arg, size+1);
	CountAdd();
}


Parameter::Parameter(const COMMON::SEF_Uchar* arg, SEF_Size_t size, COMMON::ValueType type)
{
	m_type = type;
	m_bytes.setBytes((const SEF_Uchar*)&arg, size);
	CountAdd();
}

void Parameter::serialize(COMMON::ByteArray& buffer)
{
	SEF_Size_t size = 0;
	ParamaterHeadeT header = { 0,0 };
	header.type = static_cast<unsigned char> (m_type);

	SEF_Uchar* data = m_bytes.getBytes(size);
	SwapEndian::swap2network(header.length, size);
	buffer.appendBytes((SEF_Uchar *) &header, paramHeaderSize, data, size);
}

SEF_Size_t Parameter::GetSerializeSize()
{
	return paramHeaderSize + m_bytes.getSize();
}

void Parameter::deserialize(COMMON::ByteArray& buffer)
{
	SEF_Uint32 size = 0;
	SEF_Uint32 paramaterSize = 0;
	ParamaterHeadeT header = { 0,0 };
	SEF_Uchar* data = buffer.getBytes(size);
	if (size < paramHeaderSize) 
	{
		throw "paramater message buffer error";
	}
	memcpy(&header, data, paramHeaderSize);
	m_type = (COMMON::ValueType)header.type;
	paramaterSize = SwapEndian::swap2local(header.length, paramaterSize);
	assert(size == paramaterSize + paramHeaderSize);
	m_bytes.setBytes(data + paramHeaderSize, paramaterSize);
}

SEF_Size_t Parameter::deserialize(SEF_Uchar* buffer, SEF_Size_t size)
{
	SEF_Size_t index = 0;
	SEF_Uint32 paramaterSize = 0;
	ParamaterHeadeT header = { 0,0 };
	SEF_Uchar* data = buffer;
	if (size < paramHeaderSize) 
	{
		throw "paramater message buffer error";
	}
	memcpy(&header, data, paramHeaderSize);
	m_type = (COMMON::ValueType)header.type;
	paramaterSize = SwapEndian::swap2local(header.length, paramaterSize);
	index = paramaterSize + paramHeaderSize;
	assert(size >= index);
	m_bytes.setBytes(data + paramHeaderSize, paramaterSize);
	return index;
}



SEF_String Parameter::getString()const
{
	if (m_type == ValueType::VALUE_TYPE_STRING) 
	{
		SEF_Uint32 size = 0;
		SEF_Uchar* data = m_bytes.getBytes(size);
		SEF_String res(reinterpret_cast <char *>(data));
		return res;
	}
	return SEF_String();
}

SEF_Bool Parameter::getBool()const
{
	if (m_type == ValueType::VALUE_TYPE_BOOL) 
	{
		SEF_Bool res;
		SEF_Uint32 size = 0;
		SEF_Uchar* data = m_bytes.getBytes(size);
		memcpy(&res, data, size);
		return res;
	}
	return SEF_Bool();
}

SEF_Double Parameter::getDouble()const
{
	if (m_type == ValueType::VALUE_TYPE_DOUBLE) 
	{
		SEF_Double res;
		SEF_Size_t size = 0;
		SEF_Uchar* data = m_bytes.getBytes(size);
		memcpy(&res, data, size);
		return res;
	}
	return SEF_Double();
}

SEF_Float Parameter::getFloat()const
{
	if (m_type == ValueType::VALUE_TYPE_FLOAT) 
	{
		SEF_Float res;
		SEF_Size_t size = 0;
		SEF_Uchar* data = m_bytes.getBytes(size);
		memcpy(&res, data, size);
		return res;
	}
	return SEF_Float();
}

SEF_Long Parameter::getLong()const
{
	if (m_type == ValueType::VALUE_TYPE_LONG) 
	{
		SEF_Long res;
		SEF_Size_t size = 0;
		SEF_Uchar* data = m_bytes.getBytes(size);
		memcpy(&res, data, size);
		return res;
	}
	return SEF_Long();
}

SEF_Int64 Parameter::getInt64()const
{
	if (m_type == ValueType::VALUE_TYPE_INT64) 
	{
		SEF_Int64 res;
		SEF_Size_t size = 0;
		SEF_Uchar* data = m_bytes.getBytes(size);
		memcpy(&res, data, size);
		return res;
	}
	return SEF_Int64();
}

SEF_Uint64 Parameter::getUint64()const
{
	if (m_type == ValueType::VALUE_TYPE_UINT64) 
	{
		SEF_Uint64 res;
		SEF_Size_t size = 0;
		SEF_Uchar* data = m_bytes.getBytes(size);
		memcpy(&res, data, size);
		return res;
	}
	return SEF_Uint64();
}

SEF_Uint32 Parameter::getUint32()const
{
	if (m_type == ValueType::VALUE_TYPE_UINT32) 
	{
		SEF_Uint32 res;
		SEF_Size_t size = 0;
		SEF_Uchar* data = m_bytes.getBytes(size);
		memcpy(&res, data, size);
		return res;
	}
	return SEF_Uint32();
}

SEF_Int32 Parameter::getInt32()const
{
	if (m_type == ValueType::VALUE_TYPE_INT32) 
	{
		SEF_Int32 res;
		SEF_Size_t size = 0;
		SEF_Uchar* data = m_bytes.getBytes(size);
		memcpy(&res, data, size);
		return res;
	}
	return SEF_Int32();
}

SEF_Uint16 Parameter::getUint16()const
{
	if (m_type == ValueType::VALUE_TYPE_UINT16)
	{
		SEF_Uint16 res;
		SEF_Size_t size = 0;
		SEF_Uchar* data = m_bytes.getBytes(size);
		memcpy(&res, data, size);
		return res;
	}
	return SEF_Uint16();
}

SEF_Int16 Parameter::getInt16()const
{
	if (m_type == ValueType::VALUE_TYPE_INT16) 
	{
		SEF_Int16 res;
		SEF_Size_t size = 0;
		SEF_Uchar* data = m_bytes.getBytes(size);
		memcpy(&res, data, size);
		return res;
	}
	return SEF_Int16();
}

SEF_Uchar Parameter::getUchar()const
{
	if (m_type == ValueType::VALUE_TYPE_UCHAR) 
	{
		SEF_Uchar res;
		SEF_Size_t size = 0;
		SEF_Uchar* data = m_bytes.getBytes(size);
		memcpy(&res, data, size);
		return res;
	}
	return SEF_Uchar();
}

SEF_Char Parameter::getChar()const
{
	if (m_type == ValueType::VALUE_TYPE_CHAR) 
	{
		SEF_Char res;
		SEF_Size_t size = 0;
		SEF_Uchar* data = m_bytes.getBytes(size);
		memcpy(&res, data, size);
		return res;
	}
	return SEF_Char();
}

SEF_String Parameter::toString()const
{
	SEF_Uchar* data = m_bytes.getBytes();
	SEF_Size_t size = m_bytes.getSize();

	switch (m_type)
	{
	case ValueType::VALUE_TYPE_CHAR: {
		SEF_Char v = getChar();
		return to_string(v);
	}
	case ValueType::VALUE_TYPE_UCHAR: {
		SEF_Uchar v = getUchar();
		return to_string(v);
	}
	case ValueType::VALUE_TYPE_INT16: {
		SEF_Int16 v = getInt16();
		return to_string(v);
	}
	case ValueType::VALUE_TYPE_UINT16: {
		SEF_Uint16 v = getUint16();
		return to_string(v);
	}
	case ValueType::VALUE_TYPE_INT32: {
		SEF_Int32 v = getInt32();
		return to_string(v);
	}
	case ValueType::VALUE_TYPE_UINT32: {
		SEF_Uint32 v = getUint32();
		return to_string(v);
	}
	case ValueType::VALUE_TYPE_UINT64: {
		SEF_Uint64 v = getUint64();
		return to_string(v);
	}
	case ValueType::VALUE_TYPE_INT64: {
		SEF_Int64 v = getInt64();
		return to_string(v);
	}
	case ValueType::VALUE_TYPE_LONG: {
		SEF_Long v = getLong();
		return to_string(v);
	}
	case ValueType::VALUE_TYPE_FLOAT: {
		SEF_Float v = getFloat();
		return to_string(v);
	}
	case ValueType::VALUE_TYPE_DOUBLE: {
		SEF_Double v = getDouble();
		return to_string(v);
	}
	case ValueType::VALUE_TYPE_BOOL: {
		SEF_Bool v = getBool();
		return to_string(v);
	}
	case ValueType::VALUE_TYPE_STRING: {
		return getString();
	}
	}


	return SEF_String();
}

Parameter& Parameter::operator=(const Parameter& right)
{
	if (this != &right)
	{
		m_bytes = right.m_bytes;
		m_type = right.m_type;
		CountAdd();
	}
	return *this;

}

Parameter::~Parameter()
{
	CountDel();
}

Request::Request()
{
	CountAdd();
}

Request::Request(const SEF_String& name)
{
	m_name = name;
	CountAdd();
}

Request::Request(const Request& copy)
{
	m_name = copy.getName();
	m_senderName = copy.getSender();
	m_parameters = copy.m_parameters;
	m_paramaterPredictSize = copy.m_paramaterPredictSize;
	m_marker = copy.m_marker;
	m_cseq = copy.m_cseq;
	m_byteArray = copy.m_byteArray;
	CountAdd();
}

Request::Request(SEF_String name, SEF_Bool arg0)
{
	m_name = name;
	Parameter param(arg0);
	m_parameters.push_back(Parameter(arg0));
	m_paramaterPredictSize += param.GetSerializeSize();
	CountAdd();
}

Request::Request(SEF_String name, SEF_Int64 arg0)
{
	m_name = name;
	Parameter param(arg0);
	m_parameters.push_back(Parameter(arg0));
	m_paramaterPredictSize += param.GetSerializeSize();
	CountAdd();
}

Request::Request(SEF_String name, SEF_Float arg0)
{
	m_name = name;
	Parameter param(arg0);
	m_parameters.push_back(Parameter(arg0));
	m_paramaterPredictSize += param.GetSerializeSize();
	CountAdd();
}

Request::Request(SEF_String name, SEF_String arg0)
{
	m_name = name;
	Parameter param(arg0);
	m_parameters.push_back(Parameter(arg0));
	m_paramaterPredictSize += param.GetSerializeSize();
	CountAdd();
}

Request::~Request()
{
	clear();
	CountDel();
}

void Request::clear()
{
	m_parameters.clear();
}

Request& Request::operator=(const Request& right)
{
	if (this != &right) 
	{
		setName(right.getName());
		setSender(right.getSender());
		m_parameters.clear();
		m_parameters = right.m_parameters;
		CountAdd();
	}
	return *this;
}

const SEF_String& Request::getName() const
{
	return m_name;
}

void Request::setName(const SEF_String& name)
{
	m_name = name;
}

void Request::setMarker(const COMMON::SEF_Uint32 marker)
{
	m_marker = marker;
}

SEF_Uint32 Request::getMarker()
{
	return m_marker;
}

void Request::setCseq(const COMMON::SEF_Uint16 cseq)
{
	m_cseq = cseq;
}

SEF_Uint16 Request::getCseq()
{
	return m_cseq;
}

SEF_Size_t Request::getSerializeSize()const
{
	SEF_Size_t  topSize = 0;
	SEF_Size_t  payloadLenght = 0;

	payloadLenght = m_name.size() + m_senderName.size() + m_paramaterPredictSize;
	topSize = requestHeaderSize + payloadLenght;

	return topSize;
}

void SEF::COMMON::Request::CountAdd()
{
	requestCounter++;
}

void SEF::COMMON::Request::CountDel()
{
	requestCounter--;
	if (requestCounter > 1000&& requestCounter&1000==1)
	{
		std::cout << "\n#+++++++++++1+++++++++requestCounter:" << requestCounter << std::endl;
	}
}

const SEF_String& Request::getSender() const
{
	// TODO: 在此处插入 return 语句
	return m_senderName;
}

void Request::setSender(const SEF_String& name)
{
	m_senderName = name;
}

SEF_Uint32 Request::parameterCount() const
{
	return 	m_parameters.size();
;
}

const Parameter Request::getParameter(SEF_Uint32 index) const
{
	if (index > m_parameters.size()) 
	{
		return Parameter();
	}
	return m_parameters[index];
}

ValueType Request::getParameterType(SEF_Uint32 index) const
{
	if (index > m_parameters.size()) 
	{
		return ValueType::VALUE_TYPE_NONE;
	}
	const Parameter& param = m_parameters[index];
	return param.m_type;
}

SEF_Bool Request::getBool(SEF_Uint32 index) const
{
	if (index > m_parameters.size())
	{
		return SEF_Bool();
	}
	Parameter param = m_parameters[index];
	assert(param.m_type == ValueType::VALUE_TYPE_BOOL);
	return param.getBool();
}

SEF_Char Request::getChar(SEF_Uint32 index) const
{
	if (index > m_parameters.size()) 
	{
		return SEF_Char();
	}
	Parameter param = m_parameters[index];
	assert(param.m_type == ValueType::VALUE_TYPE_CHAR);

	return param.getChar();
}

SEF_Uchar Request::getUChar(SEF_Uint32 index) const
{
	if (index > m_parameters.size()) 
	{
		return SEF_Uchar();
	}
	Parameter param = m_parameters[index];
	assert(param.m_type == ValueType::VALUE_TYPE_UCHAR);

	return param.getUchar();
}

SEF_Uint16 Request::getUInt16(SEF_Uint32 index) const
{
	if (index > m_parameters.size()) 
	{
		return SEF_Uint16();
	}
	Parameter param = m_parameters[index];
	assert(param.m_type == ValueType::VALUE_TYPE_UINT16);

	return param.getUint16();
}

SEF_Uint32 Request::getUInt32(SEF_Uint32 index) const
{
	if (index > m_parameters.size()) 
	{
		return SEF_Uint32();
	}
	Parameter param = m_parameters[index];
	assert(param.m_type == ValueType::VALUE_TYPE_UINT32);
	return param.getUint32();
}

SEF_Uint64 Request::getUInt64(SEF_Uint32 index) const
{
	if (index > m_parameters.size()) 
	{
		return SEF_Uint64();
	}
	Parameter param = m_parameters[index];
	assert(param.m_type == ValueType::VALUE_TYPE_UINT64);
	return param.getUint64();
}

SEF_Int16 Request::getInt16(SEF_Uint32 index) const
{
	if (index > m_parameters.size()) 
	{
		return SEF_Int16();
	}
	Parameter param = m_parameters[index];
	assert(param.m_type == ValueType::VALUE_TYPE_UINT32);
	return param.getInt16();
}

SEF_Int32 Request::getInt32(SEF_Uint32 index) const
{
	if (index > m_parameters.size()) 
	{
		return SEF_Int32();
	}
	Parameter param = m_parameters[index];
	assert(param.m_type == ValueType::VALUE_TYPE_INT32);
	return param.getInt32();
}

SEF_Int64 Request::getInt64(SEF_Uint32 index) const
{
	if (index > m_parameters.size()) 
	{
		return SEF_Int64();
	}
	Parameter param = m_parameters[index];
	assert(param.m_type == ValueType::VALUE_TYPE_INT64);

	return param.getInt64();
}

SEF_Float Request::getFloat32(SEF_Uint32 index) const
{
	if (index > m_parameters.size())
	{
		return SEF_Float();
	}
	Parameter param = m_parameters[index];
	assert(param.m_type == ValueType::VALUE_TYPE_FLOAT);

	return param.getFloat();
}

SEF_Double SEF::COMMON::Request::getDouble(SEF_Uint32 index) const
{
	if (index > m_parameters.size())
	{
		return SEF_Double();
	}
	Parameter param = m_parameters[index];
	assert(param.m_type == ValueType::VALUE_TYPE_DOUBLE);

	return param.getDouble();
}

const SEF_String Request::getString(SEF_Uint32 index) const
{
	if (index > m_parameters.size()) 
	{
		return SEF_String();
	}
	Parameter param = m_parameters[index];
	assert(param.m_type == ValueType::VALUE_TYPE_STRING);
	return param.getString();
}

const ByteArray Request::getByteArray(SEF_Uint32 index) const
{
	if (index > m_parameters.size()) 
	{
		return ByteArray();
	}
	Parameter param = m_parameters[index];

	return param.m_bytes;
}

void Request::add(const SEF_Bool& value)
{
	Parameter param(value);
	m_parameters.push_back(param);
	m_paramaterPredictSize += param.GetSerializeSize();
}

void Request::add(const SEF_Char& value)
{
	Parameter param(value);
	m_parameters.push_back(param);
	m_paramaterPredictSize += param.GetSerializeSize();
}

void Request::add(const SEF_Uchar& value)
{
	Parameter param(value);
	m_parameters.push_back(param);
	m_paramaterPredictSize += param.GetSerializeSize();
}

void Request::add(const SEF_Uint16& value)
{
	Parameter param(value);
	m_parameters.push_back(param);
	m_paramaterPredictSize += param.GetSerializeSize();
}

void Request::add(const SEF_Uint32& value)
{
	Parameter param(value);
	m_parameters.push_back(param);
	m_paramaterPredictSize += param.GetSerializeSize();
}

void Request::add(const SEF_Uint64& value)
{
	Parameter param(value);
	m_parameters.push_back(param);
	m_paramaterPredictSize += param.GetSerializeSize();
}

void Request::add(const SEF_Int16& value)
{
	Parameter param(value);
	m_parameters.push_back(param);
	m_paramaterPredictSize += param.GetSerializeSize();
}

void Request::add(const SEF_Int32& value)
{
	Parameter param(value);
	m_parameters.push_back(param);
	m_paramaterPredictSize += param.GetSerializeSize();
}

void Request::add(const SEF_Int64& value)
{
	Parameter param(value);
	m_parameters.push_back(param);
	m_paramaterPredictSize += param.GetSerializeSize();
}

void Request::add(const SEF_Float& value)
{
	Parameter param(value);
	m_parameters.push_back(param);
	m_paramaterPredictSize += param.GetSerializeSize();
}

void Request::add(const SEF_Double& value)
{
	Parameter param(value);
	m_parameters.push_back(param);
	m_paramaterPredictSize += param.GetSerializeSize();
}

void Request::add(const SEF_Char* value)
{
	if (value != nullptr) 
	{
		add(value, strlen(value));
	}
}

void Request::add(const SEF_String& value)
{
	Parameter param(value);
	m_parameters.push_back(param);
	m_paramaterPredictSize += param.GetSerializeSize();
}

void Request::add(const SEF_Char* value, size_t  size)
{
	Parameter param(value,size);
	m_parameters.push_back(param);
	m_paramaterPredictSize += param.GetSerializeSize();
}

void Request::add(const SEF_Uchar* value, size_t  size)
{
	Parameter param(value, size);
	m_parameters.push_back(param);
	m_paramaterPredictSize += param.GetSerializeSize();
}

void Request::add(const SEF_Uchar* value, SEF_Size_t size, ValueType type)
{
	Parameter param(value, size,type);
	m_parameters.push_back(param);
	m_paramaterPredictSize += param.GetSerializeSize();
}

void Request::add(Parameter& value)
{
	m_parameters.push_back(value);
	m_paramaterPredictSize += value.GetSerializeSize();
}

SEF_String SEF::COMMON::Request::toString()const
{
	SEF_String str;
	str += "request name:" + m_name;
	str += "\nsender name:" + m_senderName;
	for (int i = 0;i<parameterCount();i++)
	{
		str += "\nparam[" + to_string(i) + "]:" + getParameter(i).toString();
	}
	return str;
}

void Request::serialize(ByteArray& buffer)
{
	SEF_Uint32  topSize = 0;
	SEF_Uint32  payloadLenght = 0;
	bool	ret = false;

	RequestHeader header;
	baseHeaderInit(header.base_header, gVersion, m_marker, 0, 1);
	SwapEndian::swap2network(header.cseq, m_cseq);
	topSize = m_parameters.size();
	SwapEndian::swap2network(header.parameter_count, topSize);
	SwapEndian::swap2network(header.requestNameSize, (SEF_Uint16)m_name.size());
	SwapEndian::swap2network(header.senderNameSize, (SEF_Uint16)m_senderName.size());

	payloadLenght = m_name.size() + m_senderName.size() + m_paramaterPredictSize;
	SwapEndian::swap2network(header.length, payloadLenght);
	topSize = requestHeaderSize + payloadLenght;
	buffer.reMalloc(topSize);
	ret = buffer.appendBytes(&header, requestHeaderSize);
	assert(ret == true);
	ret = buffer.appendBytes(m_name.c_str(), m_name.size());
	assert(ret == true|| m_name.size()==0);

	ret = buffer.appendBytes(m_senderName.c_str(), m_senderName.size());
	assert(ret == true|| m_senderName.size()==0);

	for (Parameter param: m_parameters) 
	{
		param.serialize(buffer);
	}
	m_byteArray = buffer;
}

SEF_Size_t Request::deserialize(const ByteArray& buffer)
{
	SEF_Size_t size = 0;
	SEF_Size_t  index = 0;
	SEF_Uint32 len = 0;
	SEF_Uint16 cseq = 0;
	SEF_Uint32 parameterCount = 0;
	SEF_Uint32 parameterIndex = 0;
	SEF_Uint32 requestLen = 0;

	SEF_Uchar* data = buffer.getBytes(size);
	RequestHeader header;
	memcpy(&header, data, requestHeaderSize);
	//baseHeaderParser(data[0], header.base_header);
	index += requestHeaderSize;

	SEF_Uint16 tSendNameLen = 0;
	SEF_Uint16 tRequestNameLen = 0;
	SwapEndian::swap2local(header.senderNameSize, tSendNameLen);
	SwapEndian::swap2local(header.requestNameSize, tRequestNameLen);


	SwapEndian::swap2local(header.parameter_count, parameterCount);
	SwapEndian::swap2local(header.length, requestLen);
	SwapEndian::swap2local(header.cseq, cseq);


	string requestName((SEF_Char *)(data + index), tRequestNameLen);
	index += tRequestNameLen;
	string sendName((SEF_Char*)(data + index), tSendNameLen);
	index += tSendNameLen;
	setName(requestName);
	setSender(sendName);
	while (index < size&& parameterIndex< parameterCount)
	{
		Parameter param;
		index += param.deserialize((SEF_Uchar*)(data + index), size - index);
		m_parameters.push_back(param);
		parameterIndex++;
	}
	m_byteArray = buffer;
	return index;
}

SEF_Size_t SEF::COMMON::Request::deserialize(const COMMON::SEF_Uchar* data, SEF_Size_t size)
{
	SEF_Size_t  index = 0;
	SEF_Uint32 len = 0;
	SEF_Uint16 cseq = 0;
	SEF_Uint32 parameterCount = 0;
	SEF_Uint32 parameterIndex = 0;
	SEF_Uint32 requestLen = 0;


	RequestHeader header;
	memcpy(&header, data, requestHeaderSize);
	//baseHeaderParser(data[0], header.base_header);
	index += requestHeaderSize;

	SEF_Uint16 tSendNameLen = 0;
	SEF_Uint16 tRequestNameLen = 0;
	SwapEndian::swap2local(header.senderNameSize, tSendNameLen);
	SwapEndian::swap2local(header.requestNameSize, tRequestNameLen);


	SwapEndian::swap2local(header.parameter_count, parameterCount);
	SwapEndian::swap2local(header.length, requestLen);
	SwapEndian::swap2local(header.cseq, cseq);


	string requestName((SEF_Char*)(data + index), tRequestNameLen);
	index += tRequestNameLen;
	string sendName((SEF_Char*)(data + index), tSendNameLen);
	index += tSendNameLen;
	setName(requestName);
	setSender(sendName);
	while (index < size && parameterIndex < parameterCount)
	{
		Parameter param;
		index += param.deserialize((SEF_Uchar*)(data + index), size - index);
		m_parameters.push_back(param);
		parameterIndex++;
	}
	m_byteArray.setBytes(data, size);
	return index;
}

/*
* 
* 
* RequestQueue
*
*/

SEF::COMMON::RequestQueue::RequestQueue()
{
}

RequestQueue::RequestQueue(const SEF_String& name)
{
	m_name = name;
}

RequestQueue::RequestQueue(const RequestQueue& copy)
{
}

RequestQueue::RequestQueue(RequestQueue&& other)noexcept
{
	other.m_mutex.lock();
	m_name = other.m_name;
	m_queue = std::move(other.m_queue);

	other.m_mutex.unlock();
}

RequestQueue::~RequestQueue()
{
	m_queue.clear();
}

void RequestQueue::clear()
{
	m_queue.clear();
}

void RequestQueue::setListenState(const SEF_Bool& state)
{
	m_listenState = state;
}

const SEF_Bool& RequestQueue::getListenState() const
{
	// TODO: 在此处插入 return 语句
	return m_listenState;
}

void RequestQueue::lock()
{
	m_mutex.lock();
}

void RequestQueue::unlock()
{
	m_mutex.unlock();
}

void RequestQueue::pushBack(std::shared_ptr<Request> request)
{
	m_queue.push_back(request);
	SEF_Size_t size = request->getSerializeSize();
	m_requestBytesSize += size;
}


void RequestQueue::addRegisteredRequest(const SEF_String& requestName)
{
	m_registeredRequests.insert(requestName);
}

set<SEF_String> RequestQueue::getRegisteredRequests()
{
	return m_registeredRequests;
}

SEF_Bool RequestQueue::isRequestRegistered(const SEF_String& name)
{
	auto it = m_registeredRequests.find(name);
	if (it != m_registeredRequests.end())
	{
		return true;
	}
	return false;
}

const std::shared_ptr<Request> RequestQueue::popFront()
{
	if (m_queue.size() == 0)
	{
		return std::make_shared<Request>();
	}
	std::shared_ptr<Request> request = m_queue.front();
	m_queue.pop_front();
	m_requestBytesSize -= request->getSerializeSize();
	return request;
}

const std::shared_ptr<Request> RequestQueue::peekFront() const
{
	if (m_queue.size() == 0)
	{
		return std::make_shared<Request>();
	}
	return m_queue.front();
}

bool RequestQueue::popFront(std::shared_ptr<Request>& request)
{
	if (m_queue.size() == 0)
	{
		return false;
	}
	request = m_queue.front();
	m_queue.pop_front();
	m_requestBytesSize -= request->getSerializeSize();
	return true;
}

void RequestQueue::peekFront(std::shared_ptr<Request>& request) const
{
	if (m_queue.size() == 0)
	{
		return;
	}
	request = m_queue.front();
}

SEF_Bool RequestQueue::empty() const
{
	return m_queue.empty();
}

SEF_Uint32 RequestQueue::getSize() const
{
	return m_queue.size();
}

void RequestQueue::setName(const COMMON::SEF_String& name)
{
	m_name = name;
}

const SEF_String& RequestQueue::getName() const
{
	return m_name;
}

SEF_String RequestQueue::toString() const
{
	SEF_String str;
	int i = 0;
	str += "request name:" + m_name+"\nregisterRequestName:";
	for (const string name: m_registeredRequests)
	{
		str += "\n["+to_string(i) + "]" + name;
	}
	str += "\n\nrequest String:\n";
	for (std::shared_ptr<Request>req : m_queue)
	{
		str += "\n[" + to_string(i) + "]:" + req->toString();
	}
	return str;
}

void RequestQueue::serialize(ByteArray& buffer)
{
	SEF_Uint32  topSize = 0 ;
	SEF_Uint32  payloadLenght = m_name.size() + m_requestBytesSize;
	Request registerNameReq("register names");
	ByteArray registerNameReqBuf;

	bool	ret = false;

	RequestQueueHeaderT header;
	baseHeaderInit(header.base_header, gVersion, 1, 0, 2);
	SwapEndian::swap2network(header.queueNameSize, (SEF_Uint32)m_name.size());

	for (std::string name: m_registeredRequests)
	{
		registerNameReq.add(name);
	}
	registerNameReq.setSender(m_name);
	registerNameReq.serialize(registerNameReqBuf);
	payloadLenght += m_name.size() + m_requestBytesSize + registerNameReq.getSerializeSize();
	SwapEndian::swap2network(header.length, payloadLenght);

	topSize = payloadLenght + queueHeaderSize;
	buffer.reMalloc(topSize);

	ret = buffer.appendBytes(&header, queueHeaderSize);
	assert(ret == true);
	ret = buffer.appendBytes(m_name.c_str(), m_name.size());
	assert(ret == true);
	ret = buffer.appendBytes(registerNameReqBuf);
	assert(ret == true);

	for (std::shared_ptr<Request> request : m_queue)
	{
		ByteArray array;
		request->serialize(array);
		ret = buffer.appendBytes(array);
		assert(ret == true);
	}
}

void RequestQueue::deserialize(const ByteArray& buffer)
{
	SEF_Size_t size = 0;
	SEF_Size_t  index = 0;
	SEF_Size_t  parserSize = 0;
	SEF_Uint32 len = 0;
	SEF_Uint32 registerCount = 0;
	SEF_Uint32 requestLen = 0;
	Request registerNameReq;

	SEF_Uchar* data = buffer.getBytes(size);
	RequestQueueHeaderT header;
	memcpy(&header, data, queueHeaderSize);
	index += queueHeaderSize;

	SEF_Uint32 tQueueNameSize = 0;
	SEF_Uint32 tPayloadLen = 0;
	SwapEndian::swap2local(header.queueNameSize, tQueueNameSize);
	SwapEndian::swap2local(header.length, tPayloadLen);

	string queueName((SEF_Char*)(data + index), tQueueNameSize);
	index += tQueueNameSize;
	m_name = queueName;

	parserSize = registerNameReq.deserialize(data + index, size - index);
	index += parserSize;
	registerCount = registerNameReq.parameterCount();
	for (SEF_Uint32 i = 0;i< registerCount;i++)
	{
		assert(registerNameReq.getParameterType(i) == ValueType::VALUE_TYPE_STRING);
		m_registeredRequests.insert(registerNameReq.getString(i));
	}

	while (index != size)
	{
		std::shared_ptr<Request> request = std::make_shared<Request>();
		parserSize = request->deserialize((SEF_Uchar*)(data + index), size - index);
		index += parserSize;
		m_queue.push_back(request);
	}
}


