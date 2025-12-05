#pragma once
#include "udmLog.h"

namespace SEF
{
	namespace Log
	{
		enum class  LogLibType {
			NONE,
			CLOGGER,
		};
		class UDM_EXPORT_API UdmLogFactory {
		public:
			static UdmLog* getLogInstance();
			static bool setConfig(LogLibType type, UdmLogConfig *config);
			static void fmtLog(SEF::Log::SEFLogLevel level, const char* file, int line, const char* fmt, ...);
		};
	}
}

#define LOG_STREAM(level, file, line, log)\
do{\
    SEF::Log::UdmLog *logger = SEF::Log::UdmLogFactory::getLogInstance(); \
	if(logger)\
    logger->logWrite(level, log, file, line);\
} while (0)


#define LOGFLUSH() \
do{\
    SEF::Log::UdmLog *logger = SEF::Log::UdmLogFactory::getLogInstance(); \
	if(logger)\
    logger->loggerFlush();\
} while (0)

#define LOGT(log) LOG_STREAM(SEF::Log::SEFLogLevel::Trace, __FILENAME__, __LINE__, log)
#define LOGD(log) LOG_STREAM(SEF::Log::SEFLogLevel::Debug, __FILENAME__, __LINE__, log)
#define LOGI(log)  LOG_STREAM(SEF::Log::SEFLogLevel::Info, __FILENAME__, __LINE__, log)
#define LOGW(log)  LOG_STREAM(SEF::Log::SEFLogLevel::Warning, __FILENAME__, __LINE__, log)
#define LOGE(log) LOG_STREAM(SEF::Log::SEFLogLevel::Error, __FILENAME__, __LINE__, log)
#define LOGA(log) LOG_STREAM(SEF::Log::SEFLogLevel::Alarm, __FILENAME__, __LINE__, log)
#define LOGF(log) LOG_STREAM(SEF::Log::SEFLogLevel::Fatal, __FILENAME__, __LINE__, log)

#define LOGTEX(log) LOG_STREAM(SEF::Log::SEFLogLevel::Trace, "", __LINE__, log)
#define LOGDEX(log) LOG_STREAM(SEF::Log::SEFLogLevel::Debug, "", __LINE__, log)
#define LOGIEX(log)  LOG_STREAM(SEF::Log::SEFLogLevel::Info, "", __LINE__, log)
#define LOGWEX(log)  LOG_STREAM(SEF::Log::SEFLogLevel::Warning, "", __LINE__, log)
#define LOGEEX(log) LOG_STREAM(SEF::Log::SEFLogLevel::Error, "", __LINE__, log)
#define LOGAEX(log) LOG_STREAM(SEF::Log::SEFLogLevel::Alarm, "", __LINE__, log)
#define LOGFEX(log) LOG_STREAM(SEF::Log::SEFLogLevel::Fatal, "", __LINE__, log)

#define LOGFMTT(fmt, ...) SEF::Log::UdmLogFactory::fmtLog(SEF::Log::SEFLogLevel::Trace, __FILENAME__, __LINE__, fmt, ##__VA_ARGS__)
#define LOGFMTD(fmt, ...) SEF::Log::UdmLogFactory::fmtLog(SEF::Log::SEFLogLevel::Debug, __FILENAME__, __LINE__, fmt, ##__VA_ARGS__)
#define LOGFMTI(fmt, ...) SEF::Log::UdmLogFactory::fmtLog(SEF::Log::SEFLogLevel::Info, __FILENAME__, __LINE__, fmt, ##__VA_ARGS__)
#define LOGFMTW(fmt, ...) SEF::Log::UdmLogFactory::fmtLog(SEF::Log::SEFLogLevel::Warning, __FILENAME__, __LINE__, fmt, ##__VA_ARGS__)
#define LOGFMTE(fmt, ...) SEF::Log::UdmLogFactory::fmtLog(SEF::Log::SEFLogLevel::Error, __FILENAME__, __LINE__, fmt, ##__VA_ARGS__)
#define LOGFMTA(fmt, ...) SEF::Log::UdmLogFactory::fmtLog(SEF::Log::SEFLogLevel::Alarm, __FILENAME__, __LINE__, fmt, ##__VA_ARGS__)
#define LOGFMTF(fmt, ...) SEF::Log::UdmLogFactory::fmtLog(SEF::Log::SEFLogLevel::Fatal, __FILENAME__, __LINE__, fmt, ##__VA_ARGS__)

