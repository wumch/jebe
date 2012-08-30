
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

#define BOOST_EXCEPTION_DIABLE		// no guy should throw exceptions to me!

#include "meta.hpp"
#include "misc.hpp"
#include <utility>
#include <vector>
#include "weighted_type.hpp"

#define JEBE_FTS_WORDS_NUM			(1<<20)
#define JEBE_FTS_WORDS_HASH_BITS	(1<<17)		// about 131,072

namespace jebe {
namespace fts {

typedef std::vector<DocWeight> DocWeightList;

typedef std::vector<Word> WordList;

typedef std::vector<WordWeight> WordWeightList;

}
}
