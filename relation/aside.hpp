
#pragma once

#include "predef.hpp"
#include <boost/unordered_map.hpp>
#include "config.hpp"
#include "wordmap.hpp"

namespace jebe {
namespace rel {

class Calculater;

// misc class that hold some global-variables.
class Aside
{
public:
	static WordMap wordmap;

	static const Config* const config;

	static docnum_t curDocNum;		// current analysised document count.

	static Calculater* const caler;

	static void init()
	{

	}
};

} /* namespace rel */
} /* namespace jebe */
