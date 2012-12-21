
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

#include "meta.hpp"
#include <string>
#include <boost/static_assert.hpp>
extern "C" {
#include <limits.h>
}
#ifdef LOG_IF
#	undef LOG_IF
#endif
#ifdef LOG
#	error "macro LOG is already defined, before glog/logging.h being included."
#	undef LOG
#endif
#include <glog/logging.h>

namespace jebe {
namespace dsvd {

} /* namespace dsvd */
} /* namespace jebe */
