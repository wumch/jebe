
#pragma once

#include "predef.hpp"

namespace jebe {
namespace idf {

class BaseInput
{
public:
	BaseInput() {}

	virtual ~BaseInput() {}

	virtual void start() = 0;

	virtual const char* next() = 0;

	virtual void stop() {}
};

} /* namespace rel */
} /* namespace jebe */
