//
#include "log.h"

// using namespace std;
using namespace log4cplus;
using namespace log4cplus::helpers;

//
log4cplus::Logger Log::root;
int Log::InitLog()
{
	//
	log4cplus::initialize();
	PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log.config"));
	root = Logger::getRoot();

	//
	return 0;
}