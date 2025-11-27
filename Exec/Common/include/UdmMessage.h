#pragma once
#include <string>
#include "UdmMsg.h"

using namespace std;
using SEF::COMMON::ValueType;
using SEF::COMMON::ByteArray;
namespace UDM{
	class UDMMessage;
}

namespace SEF
{
	namespace Udm
	{
		class UDM_COMMON_API UdmMessage : public UdmMsg
		{
				
		public:
			UdmMessage();
			UdmMessage(const UdmMsg& base);
			virtual ~UdmMessage();
			bool serialize() override;
			bool serialize(ValueType type, string & value, size_t valueSize, string& topicName,
				string& variableName, string& variableUnit)override;
			bool serialize(const ByteArray &bytes, ValueType& type, const string& topicName,
				const string& variableName, const string& variableUnit) override;
			bool deserialize(void* serializeBytes, size_t size);
			UdmMessage& operator=(UdmMessage& right);
		private:
			UDM::UDMMessage* m_message = nullptr;
		};
	}
}