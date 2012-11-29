
#pragma once

#include "predef.hpp"
#include <boost/noncopyable.hpp>
#include "aside.hpp"
#include "vector.hpp"
#include "vector_impl.hpp"
#include "random.hpp"

namespace jebe {
namespace cluster {
namespace kmeans {

class CenterPicker
	: private boost::noncopyable
{
private:
	const VecList& vecs;
	const vnum_t k;

	VecList::const_iterator cursor;
	vnum_t got;

	vnum_t jmp_step;

	std::vector<vnum_t> indexes;

	fnum_t min_features;
	decimal_t min_features_rate;

public:
	CenterPicker(const VecList& vecs_, vnum_t k_, decimal_t center_min_features_rate = .0);

	CS_FORCE_INLINE bool more() const
	{
		return got < k;
	}

	const Vector* next();

private:
	void prepare();

	void calcu_min_features();

	void rand();

	CS_FORCE_INLINE bool validate(const Vector& center);
};

} /* namespace kmeans */
} /* namespace cluster */
} /* namespace jebe */
