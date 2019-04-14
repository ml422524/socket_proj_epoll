#ifndef _LOG_H_
#define _LOG_H_

#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/helpers/fileinfo.h>
#include <log4cplus/loggingmacros.h>

//
class Log{
public:
   static int InitLog();
   static log4cplus::Logger root;
};

//
#define LOG_INIT()          Log::InitLog()
#define LOG_TRACE(logEvent) LOG4CPLUS_TRACE(Log::root, logEvent)
#define LOG_DEBUG(logEvent) LOG4CPLUS_DEBUG(Log::root, logEvent)
#define LOG_INFO(logEvent)  LOG4CPLUS_INFO(Log::root, logEvent)
#define LOG_WARN(logEvent)  LOG4CPLUS_WARN(Log::root, logEvent)
#define LOG_ERROR(logEvent) LOG4CPLUS_ERROR(Log::root, logEvent)
#define LOG_FATAL(logEvent) LOG4CPLUS_FATAL(Log::root, logEvent)

#endif