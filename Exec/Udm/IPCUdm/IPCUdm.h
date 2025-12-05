#pragma once
#include "Udm.h"

namespace SEF
{
	namespace Udm
	{
		class IPCUdm : public Udm
		{
		public:
			IPCUdm();
			~IPCUdm();
			virtual bool init() = 0;
			virtual void read() = 0;
			virtual void commit() = 0;
			virtual bool publish(bool& output, const SEF_String& topicName, const SEF_String& variableName, const  SEF_String& variableUnit) = 0;
			virtual bool publish(int& output, const SEF_String& topicName, const  SEF_String& variableName, const  SEF_String& variableUnit) = 0;
			virtual bool publish(float& output, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit) = 0;
			virtual bool publish(double& output, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit) = 0;
			virtual bool publish(SEF_String& output, const SEF_String& topicName, const SEF_String& variableName, const  SEF_String& variableUnit) = 0;
			virtual bool subscribe(bool& input, const SEF_String& topicName, const SEF_String& variableName, const  SEF_String& variableUnit) = 0;
			virtual bool subscribe(int& input, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit) = 0;
			virtual bool subscribe(float& input, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit) = 0;
			virtual bool subscribe(double& input, const SEF_String& topicName, const SEF_String& variableName, const SEF_String& variableUnit) = 0;
			virtual bool subscribe(SEF_String& input, const SEF_String& topicName, const SEF_String& variableName, const  SEF_String& variableUnit) = 0;
		};
	}
}