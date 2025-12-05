#include "log.h"
#include "udmCLogger.h"
#include <stdarg.h>
using namespace SEF::Log;

static UdmLogConfig* gLogConfig = nullptr;
static LogLibType gLibType;
UdmLog* UdmLogFactory::getLogInstance()
{
	if (gLogConfig == nullptr) 
	{
		fprintf(stderr, "log configure have not set");
	}
	if (gLibType == LogLibType::CLOGGER)
	{
		return udmCLogger::getLogInstance(*gLogConfig);
	}
	return nullptr;
}

bool SEF::Log::UdmLogFactory::setConfig(LogLibType type, UdmLogConfig* config)
{
	if (gLogConfig == nullptr) 
	{
		gLogConfig = new UdmLogConfig(config);
		gLibType = type;
		return true;
	}
	else 
	{
		return false;
	}

}

void SEF::Log::UdmLogFactory::fmtLog(SEF::Log::SEFLogLevel level, const char* file, int line, const char* fmt, ...)
{
    SEF::Log::UdmLog *logger = SEF::Log::UdmLogFactory::getLogInstance();

	if (logger) 
	{
		va_list outer;
		va_start(outer, fmt);
		char buf[2024] = { 0 };
		vsnprintf_s(buf, 2024, 2023, fmt, outer);
		va_end(outer);
		logger->logWrite(level, buf, file, line);
	}
	else
	{
		va_list outer;
		va_start(outer, fmt);
		char buf[1024] = { 0 };
		vprintf(fmt, outer);
		va_end(outer);
	}

}

