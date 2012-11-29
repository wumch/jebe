
#pragma once

#include "predef.hpp"
#include <cmath>
#include "aside.hpp"

namespace jebe {
namespace cluster {
namespace kmeans {

// guess count of non-zeros for Cluster who contains @vnum vectors.
static inline fnum_t guess_nonzeros(vnum_t vnum)
{
	return std::min(Aside::totalFeatureNum, static_cast<fnum_t>((CS_BLIKELY(vnum > 0) ? (1 + std::log(vnum)) : 1) * Aside::config->reserve_fnum));
}

} /* namespace kmeans */
} /* namespace cluster */
} /* namespace jebe */
