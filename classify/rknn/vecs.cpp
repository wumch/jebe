
#include "vecs.hpp"

namespace jebe {
namespace classify {
namespace rknn {

void Clses::deliver(const SamplePool& samples)
{
	clses.resize(cls_num);
	for (SamplePool::const_iterator it = samples.cbegin(); it != samples.cend(); ++it)
	{
		if (CS_BUNLIKELY(!(it->belong_cls < cls_num)))
		{
			CS_DIE("belong_cls:" << it->belong_cls << ", cls_num:" << cls_num);
		}
		clses[it->belong_cls].attach(&*it);
	}
}

} /* namespace rknn */
} /* namespace classify */
} /* namespace jebe */
