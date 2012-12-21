
#include "aside.hpp"
#include <fstream>
#include <iostream>
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
}

void Aside::initLogger()
{
	google::InitGoogleLogging(config->argv_first);
	google::SetLogDestination(google::INFO, config->logfile.string().c_str());
}

} /* namespace svd */
} /* namespace dsvd */
} /* namespace jebe */
