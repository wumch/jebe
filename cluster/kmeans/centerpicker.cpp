
#include "centerpicker.hpp"
#include <boost/dynamic_bitset.hpp>
#include <ctime>
#include "aside.hpp"

namespace jebe {
namespace cluster {
namespace kmeans {

CenterPicker::CenterPicker(const VecList& vecs_, vnum_t k_)
	: vecs(vecs_), k(k_), got(0),  indexes(k)
{
	prepare();
}

bool CenterPicker::more() const
{
	return got < k;
}

const Vector* CenterPicker::next()
{
	return &vecs[got++];
}

void CenterPicker::prepare()
{
	rand();
}

void CenterPicker::rand()
{
	indexes.resize(k, 0);
	staging::RandomAssister<vnum_t> random(0, Aside::totalVecNum - 1, std::time(NULL));
	boost::dynamic_bitset<uint64_t> recorder(Aside::totalVecNum);
	for (vnum_t i = 0, idx = 0; i < k; )
	{
		idx = random.rand();
		if (!recorder.test(idx))
		{
			indexes[i++] = idx;
		}
	}
}

} /* namespace kmeans */
} /* namespace cluster */
} /* namespace jebe */
