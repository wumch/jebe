
#pragma once

#include "predef.hpp"
#include <leveldb/db.h>

namespace jebe {
namespace qdb {

class Storage {
public:
	Storage();

	virtual ~Storage();
};

} /* namespace cws */
} /* namespace jebe */
