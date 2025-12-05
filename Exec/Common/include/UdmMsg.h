#pragma once
#include <string>
#include "base.h"
#include "ByteArray.h"
using namespace std;
using SEF::COMMON::ValueType;
using SEF::COMMON::ByteArray;
using namespace SEF::COMMON;
namespace SEF
{
	namespace Udm
	{
		#define JSON_VAULE_TYPE_OBJ "value_type"
		#define JSON_VAULE_OBJ		"value"
		#define JSON_TOPIC_OBJ	"topic"
		#define JSON_VAR_NAME	"var"
		#define JSON_VAR_UNIT	"unit"

		enum class LockStat {
			NONE = 0,
			LOCK,
			UNLOCK,
		};

		class UDM_COMMON_API UdmMsg
		{
		public:
			UdmMsg();
			UdmMsg(const UdmMsg& msg);
			virtual ~UdmMsg();

			virtual bool serialize(ValueType type, string& value, size_t valueSize, string& topicName,
										string &variableName, string &variableUnit);
			virtual const void* GetBytes(SEF_Size_t& size);
			virtual const ByteArray& GetByteArray()const;
			virtual bool serialize();
			virtual bool serialize(const ByteArray& bytes, ValueType& type, const string &topicName,
										const string &variableName, const string &variableUnit);
			virtual bool deserialize(void* serializeBytes, size_t size);
			ValueType GetMsgType() const;
			const ByteArray * GetValue() const;
			const SEF_String& GetValueString() const;
			size_t	GetValueSize() const;
			SEF_String GetTopicName() const;
			SEF_String GetVariableName() const;
			SEF_String GetVariableUnit() const;
			SEF_String GetSourceName() const;
			SEF_Uint64 GetTimestamp() const;
			LockStat	GetLockStat() const;
			void SetTopicName(const string &v);
			void SetVariableName(const string &v);
			void SetVariableUnit(const string &v);
			void SetMsgType(ValueType type);
			void SetSourceName(const string &sourceName);
			void SetTimestamp(SEF_Uint64 timestamp);
			void SetValue(const string& v);
			void SetValue(const ByteArray* v);
			void SetSerializeBytes(const ByteArray& v);
			void SetLockStat(LockStat lockStat);
		protected:
			ByteArray m_serializeBytes;

			ValueType m_type;
			ByteArray m_value;
			SEF_String	  m_valueString;
			size_t	m_valueSize = 0;
			SEF_String	m_topicName;
			SEF_String	m_variableName;
			SEF_String m_variableUnit;
			SEF_String m_sourceName;
			SEF_Uint64 m_timestamp = 0;
			LockStat	m_lock;
		};
	}
}