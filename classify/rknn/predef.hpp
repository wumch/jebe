
#pragma once

#define _JEBE_ENABLE_OPENMP			1
#define _JEBE_ENABLE_AUTO_RECALC	0

#include "../predef.hpp"
#include <vector>
#include <boost/static_assert.hpp>

namespace jebe {
namespace classify {
namespace rknn {

typedef decimal_t sim_t;

typedef std::vector<clsid_t> ClsIdList;

} /* namespace rknn */
} /* namespace classify */
} /* namespace jebe */
