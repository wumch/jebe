
#include "centerpicker.hpp"
#include <boost/dynamic_bitset.hpp>
#include <ctime>
#include "aside.hpp"

namespace jebe {
namespace cluster {
namespace kmeans {

CenterPicker::CenterPicker(const VecList& vecs_, vnum_t k_, decimal_t center_min_features_rate)
	: vecs(vecs_), k(k_), got(0),  indexes(k), min_features_rate(center_min_features_rate)
{
	prepare();
}

const Vector* CenterPicker::next()
{
	return &vecs[got++];
}

void CenterPicker::prepare()
{
	rand();
	calcu_min_features();
}

void CenterPicker::rand()
{
	indexes.resize(k, 0);
	staging::RandomAssister<vnum_t> random(0, Aside::totalVecNum - 1, std::time(NULL));
	boost::dynamic_bitset<uint64_t> recorder(Aside::totalVecNum);
	for (vnum_t i = 0, idx = 0; i < k; )
	{
		idx = random.rand();
		if (!recorder.test(idx) && validate(vecs[idx]))
		{
			indexes[i++] = idx;
			recorder.set(idx, true);
		}
	}
}

void CenterPicker::calcu_min_features()
{
	std::vector<fnum_t> vals;
	vals.reserve(vecs.size());
	for (VecList::const_iterator it = vecs.begin(); it != vecs.end(); ++it)
	{
		vals.push_back((*it)->nnz());
	}
	std::sort(vals.begin(), vals.end());
	min_features = vals[vals.size() * min_features_rate];
}

bool CenterPicker::validate(const Vector& center)
{
	return center->nnz() >= min_features;
}

} /* namespace kmeans */
} /* namespace cluster */
} /* namespace jebe */
