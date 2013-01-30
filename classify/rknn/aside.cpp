
#include "aside.hpp"
#include <fstream>
#include <iostream>
#include <glog/logging.h>
#include "config.hpp"
#include "../rconfig.hpp"
#include "calculator.hpp"
#include "vecs.hpp"

#define __JEBE_PATTEN_LINE_SIZE_MAX	256

namespace jebe {
namespace classify {
namespace rknn {

const Config* const Aside::config = Config::getInstance();
RConfig* const Aside::rconfig = RConfig::getInstance();
SamplePool Aside::samples;
Clses* const Aside::clses = new Clses;

clsnum_t Aside::cls_num;		// total <Cls> num.
fnum_t Aside::totalFeatureNum;

void Aside::init()
{
	initLogger();
}

void Aside::initLogger()
{
	google::InitGoogleLogging(config->argv_first);
	google::SetLogDestination(google::INFO, config->logfile.string().c_str());
}

void Aside::overrideConfig()
{
	totalFeatureNum = rconfig->total_fnum;
}

} /* namespace rknn */
} /* namespace classify */
} /* namespace jebe */

#undef __JEBE_PATTEN_LINE_SIZE_MAX
