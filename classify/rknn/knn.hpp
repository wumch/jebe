
#pragma once

#include "predef.hpp"

namespace jebe {
namespace classify {
namespace rknn {

class Knn
{
public:
	Knn();

	virtual ~Knn();

	clsid_t classify() const;

protected:

};

} /* namespace rknn */
} /* namespace classify */
} /* namespace jebe */
