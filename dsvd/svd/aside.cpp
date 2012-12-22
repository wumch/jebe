
#include "aside.hpp"
#include <fstream>
#include <iostream>
extern "C" {
#include <unistd.h>
}
#include <boost/date_time/local_time/local_time.hpp>
#include <glog/logging.h>
#include "config.hpp"

namespace jebe {
namespace dsvd {
namespace svd {

const Config* const Aside::config = Config::getInstance();

void Aside::init()
{
	initLogger();
	ready();
}

void Aside::initLogger()
{
	google::InitGoogleLogging(config->argv_first);
	google::SetLogDestination(google::INFO, config->logfile.string().c_str());
}

void Aside::ready()
{
	if (config->as_daemon)
	{
		CS_ABORT_IF(daemon(1, 0) != 0);
	}
}

} /* namespace svd */
} /* namespace dsvd */
} /* namespace jebe */
