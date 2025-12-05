#include "udmCLogger.h"
#include <stdarg.h>
#include <thread>
#include <sstream>
#include "c-logger/logger.h"
#ifdef VLD_DEBUG
#include "vld.h"
#endif

using namespace SEF::Log;
using namespace SEF::COMMON;

UdmLog* udmCLogger::getLogInstance(const UdmLogConfig& config)
{
	static udmCLogger* instance = new udmCLogger(config);
	return instance;
}



void udmCLogger::logWrite(SEFLogLevel level, const std::string& log, const char* file, int line)
{
	//logger_log((LogLevel)level, file, line, "%s", log.data());
	//pageCount++;
	//if (pageCount % autoFlushPage == 0) 
	//{
	//	loggerFlush();
	//}
	tagLogInfo *info = new tagLogInfo();
	info->level = level;
	info->file = file;
	info->line = line;

	std::stringstream ss;
	ss << std::this_thread::get_id();
	info->log = "[tid "+ ss.str() +"] "+log;
	m_logQueue.push(info);

}

void SEF::Log::udmCLogger::loggerFlush()
{
	logger_flush();
}

udmCLogger::udmCLogger(const UdmLogConfig& config)
{
	logger_initConsoleLogger(stderr);
	logger_initFileLogger(config.m_path.c_str(), config.m_maxFileSize, config.m_maxFileCount);
	switch (config.m_level) 
	{
		case SEFLogLevel::Trace: {
			logger_setLevel(LogLevel_TRACE); 
			break;
		}
		case SEFLogLevel::Debug: {
			logger_setLevel(LogLevel_DEBUG);
			break;
		}
		case SEFLogLevel::Info: {
			logger_setLevel(LogLevel_INFO);
			break;
		}
		case SEFLogLevel::Warning: {
			logger_setLevel(LogLevel_WARN);
			break;
		}
		case SEFLogLevel::Error: {
			logger_setLevel(LogLevel_ERROR);
			break;
		}
		case SEFLogLevel::Fatal: {
			logger_setLevel(LogLevel_FATAL);
			break;
		}
	}
	m_thread = new std::thread(&udmCLogger::writerThread, this);

}

void SEF::Log::udmCLogger::writerThread()
{
	tagLogInfo *log = nullptr;
	while (m_logQueue.pop(log))
	{
		logger_log((LogLevel)log->level, log->file.c_str(), log->line, "%s", log->log.c_str());
		pageCount++;
		if (pageCount % autoFlushPage == 0) 
		{
			loggerFlush();
		}
		delete log;
	}
}
