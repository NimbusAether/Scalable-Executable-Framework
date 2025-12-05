#pragma once
#include "NetUdm.h"

namespace SEF
{
	namespace Udm
	{
		class SocketAPIUdm : public NetUdm
		{
		public:
			SocketAPIUdm();
			~SocketAPIUdm();
			bool init();
			void read() override;
			void commit() override;
			bool publish(bool& output, const SEF_String& topicName, const SEF_String& variableName, const  SEF_String& variableUnit) override;
			bool publish(int& output, const SEF_String& topicName, const  SEF_String& variableName, const  SEF_String& variableUnit) override;
			bool publish(float& output, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit) override;
			bool publish(double& output, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit) override;
			bool publish(SEF_String& output, const SEF_String& topicName, const SEF_String& variableName, const  SEF_String& variableUnit) override;
			bool subscribe(bool& input, const SEF_String& topicName, const SEF_String& variableName, const  SEF_String& variableUnit) override;
			bool subscribe(int& input, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit) override;
			bool subscribe(float& input, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit) override;
			bool subscribe(double& input, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit) override;
			bool subscribe(SEF_String& input, const SEF_String& topicName, const SEF_String& variableName, const  SEF_String& variableUnit) override;
			bool subscribe(NotifyCb* cb, const VarQueryInfo& index) override;
			bool sendRequest(const ByteArray& bytes, const RequestQueryInfo& info)override;
			bool recvRequest(const RequestQueryInfo& info, RequestCb * cb)override;
		};
	}
}