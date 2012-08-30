
#pragma once

#include "predef.hpp"
#include <leveldb/db.h>

namespace jebe {
namespace fts {

class Storage {
public:
	Storage();

	virtual ~Storage();
};

} /* namespace cws */
} /* namespace jebe */
