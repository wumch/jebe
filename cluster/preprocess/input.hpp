
#pragma once

#include "predef.hpp"
#include "document.hpp"

namespace jebe {
namespace cluster {
namespace preprocess {

class BaseInput
{
public:
	BaseInput() {}

	virtual ~BaseInput() {}

	virtual void start() = 0;

	virtual InDocument next() = 0;

	virtual bool next(char* heap) = 0;

	virtual void stop() {}
};

} /* namespace preprocess */
} /* namespace cluster */
} /* namespace jebe */
