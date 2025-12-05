#pragma once
#include <string>
#include "config.h"
using namespace std;
using namespace SEF::Config;
namespace SEF
{
	namespace UdmService
	{
		enum class RunningStat {
			SER_STAT_NONE = 0,
			SER_STAT_INIT,
			SER_STAT_START,
			SER_STAT_STOP,
			SER_STAT_TERM,
		};
		struct TopicItemInfo {
			string topicName;
			string variableName;
		};



		class UDM_EXPORT_API UdmService
		{
		public:
			UdmService();
			virtual ~UdmService();
			virtual bool init(const char* config) = 0;
			virtual bool init(unsigned int frequency, unsigned int timeout = 10) = 0;

			virtual bool addAddress(const UdmAddress& info) = 0;
			virtual bool start() = 0;
			virtual void stop() = 0;


			RunningStat GetStat();
			unsigned int getFrequency();
			const string getAddressUrl();
		protected:
			/*服务地址*/
			string m_ipAddress;
			unsigned short m_port;
			string m_addressUrl;
			/*pulse 脉冲频率*/
			unsigned int	m_frequency;
			/*客户端链接超时时间*/
			unsigned int m_timeout;
			/*服务运行状态*/
			RunningStat m_stat;

		};
	}
}