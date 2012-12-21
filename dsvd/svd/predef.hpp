
#pragma once

#ifndef CS_DEBUG
#	ifndef NDEBUG
#		define CS_DEBUG				1
#	else
#		define CS_DEBUG				0
#	endif
#endif
#define CS_LOG_ON					0

#define CS_USE_WCS					0

#define BOOST_EXCEPTION_DIABLE		// no guy can throw exceptions to me!

#include "../predef.hpp"

namespace jebe {
namespace dsvd {
namespace convert {

} /* namespace convert */
} /* namespace dsvd */
} /* namespace jebe */
