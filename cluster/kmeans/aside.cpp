
#include "aside.hpp"
#include <fstream>
#include <iostream>
#include <glog/logging.h>
#include "config.hpp"
#include "../rconfig.hpp"
#include "calculater.hpp"

#define __JEBE_PATTEN_LINE_SIZE_MAX	256

namespace jebe {
namespace cluster {
namespace kmeans {

const Config* const Aside::config = Config::getInstance();
RConfig* const Aside::rconfig = RConfig::getInstance();
VecList Aside::vecs;

Calculater* Aside::caler;

vnum_t Aside::totalVecNum;
vnum_t Aside::curVecNum;		// current analysised document count.

fnum_t Aside::totalFeatureNum;

void Aside::init()
{
	initLogger();
	caler = new Calculater;
}

void Aside::initLogger()
{
	google::InitGoogleLogging(config->argv_first);
	google::SetLogDestination(google::INFO, config->logfile.string().c_str());
}

void Aside::overrideConfig()
{
	Config* const config = Config::getInst();
	config->supposed_lowest_k = rconfig->supposed_lowest_k;
	config->reserve_fnum = rconfig->reserve_fnum;
	totalVecNum = rconfig->total_vnum;
	totalFeatureNum = rconfig->total_fnum;
	caler->reset_supposed_k(config->supposed_lowest_k);
}

} /* namespace kmeans */
} /* namespace cluster */
} /* namespace jebe */

#undef __JEBE_PATTEN_LINE_SIZE_MAX
