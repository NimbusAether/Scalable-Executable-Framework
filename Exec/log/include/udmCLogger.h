#pragma once

#include <thread>
#include "udmLog.h"
#include "SafeQueue.h"
namespace SEF
{
	namespace Log
	{
		struct tagLogInfo {
			SEFLogLevel level;
			COMMON::SEF_String log;
			COMMON::SEF_String file;
			COMMON::SEF_Int line;
		};

		class UDM_EXPORT_API udmCLogger :public UdmLog {
		public:
			static UdmLog* getLogInstance(const UdmLogConfig& config);
			void logWrite(SEFLogLevel level, const std::string& log, const char* file, int line) override;
			void loggerFlush() override;
		private:
			udmCLogger(const UdmLogConfig& config);
			void writerThread();
			int pageCount = 0;
			std::thread* m_thread = nullptr;
			COMMON::SafeQueue<tagLogInfo*> m_logQueue;
		};
	}
}