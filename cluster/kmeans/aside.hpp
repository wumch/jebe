
#pragma once

#include "predef.hpp"
#include <boost/ptr_container/ptr_vector.hpp>
#include "config.hpp"
#include "../rconfig.hpp"

namespace jebe {
namespace cluster {
namespace kmeans {

class Vector;
class Calculater;

typedef boost::ptr_vector<Vector> VecList;

// misc class that hold some global-variables.
class Aside
{
public:
	static VecList vecs;

	static const Config* const config;
	static RConfig* const rconfig;

	static Calculater* caler;

	static vnum_t totalVecNum;
	static vnum_t curVecNum;		// current analysised document count.

	static fnum_t totalFeatureNum;

	static void init();

protected:
	static void initLogger();

	static void initWordList();

public:
	static fnum_t wordsNum();

	static void overrideConfig();
};

} /* namespace kmeans */
} /* namespace cluster */
} /* namespace jebe */
