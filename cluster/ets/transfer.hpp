
#pragma once

#include "predef.hpp"
#include "marve_holder.hpp"
#include "input_document.hpp"

namespace jebe {
namespace cluster {
namespace ets {

class Transfer
{
private:
	MarveHolder holder;

public:
	void trans(const InDocument& indoc, FeatureList& flist);
};

} /* namespace ets */
} /* namespace cluster */
} /* namespace jebe */
