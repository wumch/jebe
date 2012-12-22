
#pragma once

#include "predef.hpp"
#include "config.hpp"

namespace jebe {
namespace dsvd {
namespace svd {

// misc class that hold some global-variables.
class Aside
{
public:
	static const Config* const config;

	static void init();

protected:
	static void initLogger();

	static void ready();
};

} /* namespace svd */
} /* namespace dsvd */
} /* namespace jebe */
