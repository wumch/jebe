
#pragma once

#include "predef.hpp"
#include <vector>
#include <cmath>
#include <boost/numeric/ublas/vector_sparse.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <Eigen/Sparse>
#include "hash.hpp"
#include "math.hpp"
#include "aside.hpp"
#include "../document.hpp"

namespace jebe {
namespace cluster {
namespace kmeans {

typedef FeatureList InputVector;

typedef boost::numeric::ublas::map_array<fid_t, fval_t> FeatureArray;
typedef boost::numeric::ublas::mapped_vector<fval_t, FeatureArray> RawVector;

class VectorMeta
{
public:
	static const clsid_t unknown_cls = 0;
	mutable clsid_t belong_cls;

	VectorMeta()
		: belong_cls(unknown_cls)
	{}

	VectorMeta(vid_t id_)
		: belong_cls(unknown_cls)
	{}

	VectorMeta(vid_t id_, clsid_t belong_cls_)
		: belong_cls(belong_cls_)
	{}

	VectorMeta(const VectorMeta& meta)
		: belong_cls(meta.belong_cls)
	{}
};

// wrap from RawVector, for flexible and optimization.
class Vector: public VectorMeta
{
public:
	RawVector rv;

public:
	Vector()
		: rv(Aside::totalFeatureNum, Aside::config->reserve_fnum)
	{}

	/*
	 * store only the result of GuiYiHua.
	 * GuiYiHua(vector) = vector / mod(vec).
	 */
	explicit Vector(const Document& doc)
		: VectorMeta(doc.id), rv(Aside::totalFeatureNum, doc.flist.size())
	{
		init_from_iv(doc.flist);
	}

	explicit Vector(vid_t id_, const RawVector& rv_)
		: VectorMeta(id_), rv(rv_)
	{
		rv.assign(rv_);
	}

	// fix the copy-constructor of boost::mapped_vector.
	Vector(const Vector& vec)
		: VectorMeta(vec.belong_cls), rv(vec.rv.size(), vec.rv.nnz())
	{
		rv.assign(vec.rv);
	}

	void reset(const RawVector& rv_)
	{
		rv.assign(rv_);
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

private:
	void init_from_iv(const InputVector& vec)
	{
		copy_from_iv(vec);
	}

private:
	void copy_from_iv(const InputVector& vec);
};

class VecFactory
{
public:
	static Vector* create(const Document& doc)
	{
		return new Vector(doc);
	}
};

} /* namespace kmeans */
} /* namespace cluster */
} /* namespace jebe */
