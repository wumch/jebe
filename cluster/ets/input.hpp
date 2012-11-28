
#pragma once

#include "predef.hpp"
#include "input_document.hpp"

namespace jebe {
namespace cluster {
namespace ets {

class BaseInput
{
public:
	BaseInput() {}

	virtual ~BaseInput() {}

	virtual void start() = 0;

	virtual InDocument* next() = 0;

	virtual bool next(char* heap) = 0;

	virtual void stop() {}

	virtual bool more() const {return true;};
};

} /* namespace ets */
} /* namespace cluster */
} /* namespace jebe */
