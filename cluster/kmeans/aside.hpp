
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

enum Action {
	wrong = 0,
	tell_config = 1,
	tell_total = 2,
	send_doc = 10,
	collected = 11,
	calculated = 12,
	thats_all = 3,
};

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
