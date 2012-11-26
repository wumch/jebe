
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

public:
	CenterPicker(const VecList& vecs_, vnum_t k_);

	CS_FORCE_INLINE bool more() const;

	const Vector* next();

private:
	void prepare();

	void rand();
};

} /* namespace kmeans */
} /* namespace cluster */
} /* namespace jebe */
