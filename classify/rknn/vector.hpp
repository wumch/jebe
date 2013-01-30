
#pragma once

#include "predef.hpp"
#include <vector>
#include <cmath>
#include <boost/noncopyable.hpp>
#include <Eigen/Sparse>
#include "hash.hpp"
#include "math.hpp"
#include "aside.hpp"
#include "../inputvector.hpp"

namespace jebe {
namespace classify {
namespace rknn {

typedef Eigen::SparseVector<fval_t> RawVector;

// wrap from RawVector, for flexible and optimization.
class Vector: private boost::noncopyable
{
public:
	RawVector rv;

public:
	Vector()
		: rv(Aside::totalFeatureNum)
	{}

	/*
	 * store only the normalized result.
	 * normalize(vector) = vector / mod(vec).
	 */
	explicit Vector(const InputVector& iv)
		: rv(Aside::totalFeatureNum)
	{
		rv.resizeNonZeros(iv.flist.size());
		init_rv_from_flist(iv.flist);
	}

	explicit Vector(const InputVector& iv, bool normalized)
		: rv(Aside::totalFeatureNum)
	{
		if (normalized)
		{
			init_rv_from_flist(iv.flist);
		}
		else
		{
			init_rv_from_flist_normalize(iv.flist);
		}
	}

	explicit Vector(const RawVector& rv_)
		: rv(rv_)
	{}

	void reset(const RawVector& rv_)
	{
		rv = rv_;
	}

protected:
	void init_rv_from_flist_normalize(const FeatureList& flist)
	{
		decimal_t square_sum;
		for (FeatureList::const_iterator it = flist.begin(); it != flist.end(); ++it)
		{
			square_sum += staging::square(it->fval);
		}
		square_sum = std::sqrt(square_sum);

		rv.resizeNonZeros(flist.size());
		for (FeatureList::const_iterator it = flist.begin(); it != flist.end(); ++it)
		{
			rv.coeffRef(it->fid) = it->fval / square_sum;
		}
	}

	void init_rv_from_flist(const FeatureList& flist)
	{
		rv.resizeNonZeros(flist.size());
		for (FeatureList::const_iterator it = flist.begin(); it != flist.end(); ++it)
		{
			rv.coeffRef(it->fid) = it->fval;
		}
	}

public:
	CS_FORCE_INLINE RawVector* operator->()
	{
		return &rv;
	}
	CS_FORCE_INLINE const RawVector* operator->() const
	{
		return &rv;
	}

	CS_FORCE_INLINE RawVector& operator*()
	{
		return rv;
	}
	CS_FORCE_INLINE const RawVector& operator*() const
	{
		return rv;
	}

	CS_FORCE_INLINE operator RawVector() const
	{
		return rv;
	}
};

class SampleMeta
{
public:
	static const clsid_t unknown_cls = 0;
	mutable clsid_t belong_cls;

	SampleMeta()
		: belong_cls(unknown_cls)
	{}

	SampleMeta(clsid_t belong_cls_)
		: belong_cls(belong_cls_)
	{}
};

class Sample: public SampleMeta, public Vector
{
	using SampleMeta::unknown_cls;
public:
	Sample(const InputSample& s)
		: SampleMeta(s.belong_cls), Vector(s, true)
	{}
};

} /* namespace rknn */
} /* namespace classify */
} /* namespace jebe */
