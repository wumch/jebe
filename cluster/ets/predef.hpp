
#pragma once

#define _JEBE_WORD_MAP_HASH_BITS		19
#define __JEBE_PATTEN_LINE_SIZE_MAX		256
#define __JEBE_PATTEN_IDF_DELIMITER		"\t"

#include "../predef.hpp"
#include <string>
#include <boost/static_assert.hpp>

namespace jebe {
namespace cluster {
namespace ets {

typedef uint8_t byte_t;
typedef fid_t wordid_t;
typedef fnum_t wordnum_t;
typedef fval_t weight_t;
typedef vid_t docid_t;
typedef vnum_t docnum_t;
typedef vnum_t atimes_t;

typedef uint32_t tsize_t;

typedef std::string Word;

} /* namespace ets */
} /* namespace cluster */
} /* namespace jebe */
