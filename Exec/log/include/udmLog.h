#pragma once
#include <string>
#include "base.h"


#if defined(_WIN32) || defined(_WIN64)
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif /* defined(_WIN32) || defined(_WIN64) */

namespace SEF
{
	namespace Log
	{
		enum class SEFLogLevel
		{
			Trace = 0,
			Debug = 1,
			Info = 2,
			Warning = 3,
			Error = 4,
			Alarm = 5,
			Fatal = 6
		};
		struct UdmLogConfig {
			std::string m_path;
			SEFLogLevel m_level;
			COMMON::SEF_Size_t m_maxFileSize = 20*1024*1024;
			COMMON::SEF_Uint16 m_maxFileCount = 10;
			UdmLogConfig()
			{

			}
			UdmLogConfig(UdmLogConfig* config)
			{
				m_path = config->m_path;
				m_level = config->m_level;
				m_maxFileCount = config->m_maxFileCount;
				m_maxFileSize = config->m_maxFileSize;
			}
		};

		class UDM_EXPORT_API UdmLog {
		public:
			virtual void logWrite(SEFLogLevel level, const char* file, int line, const char* fmt, ...) {};
			virtual void logWrite(SEFLogLevel level,const std::string& log, const char* file, int line) = 0;
			virtual void loggerFlush() = 0;
		protected:
			int autoFlushPage = 100;
		};
	}
}