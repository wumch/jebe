
#pragma once

#include "predef.hpp"
#include <boost/ptr_container/ptr_vector.hpp>
#include "config.hpp"
#include "../rconfig.hpp"

namespace jebe {
namespace classify {
namespace rknn {

class Sample;
class Clses;
typedef boost::ptr_vector<Sample> SamplePool;

// misc class that hold some global-variables.
class Aside
{
public:
	static SamplePool samples;

	static const Config* const config;
	static Clses* const clses;

	static RConfig* const rconfig;

	static fnum_t totalFeatureNum;

	static clsnum_t cls_num;

	static void init();

protected:
	static void initLogger();

	static void initWordList();

public:
	static fnum_t wordsNum();

	static void overrideConfig();
};

} /* namespace rknn */
} /* namespace classify */
} /* namespace jebe */
