
#pragma once

#include "predef.hpp"
#include <vector>
#include <boost/numeric/ublas/vector_sparse.hpp>
#include <boost/numeric/ublas/io.hpp>
#include "hash.hpp"
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
	vid_t id;
	mutable clsid_t belong_cls;

	VectorMeta()
		: id(0), belong_cls(0)
	{}

	VectorMeta(vid_t id_)
		: id(id_), belong_cls(0)
	{}

	VectorMeta(vid_t id_, clsid_t belong_cls_)
		: id(id_), belong_cls(belong_cls_)
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
	{}

	// fix the copy-constructor of boost::mapped_vector.
	Vector(const Vector& vec)
		: VectorMeta(vec.id, vec.belong_cls), rv(vec.rv.size(), vec.rv.nnz())
	{
		rv.assign(vec.rv);
	}

	void reset(const RawVector& rv_)
	{
		rv = rv_;
	}

public:
	CS_FORCE_INLINE RawVector* operator->()
	{
		return rv;
	}
	CS_FORCE_INLINE const RawVector* operator->() const
	{
		return rv;
	}

	CS_FORCE_INLINE RawVector& operator*()
	{
		return &rv;
	}
	CS_FORCE_INLINE const RawVector& operator*() const
	{
		return &rv;
	}

	CS_FORCE_INLINE operator RawVector()
	{
		return rv;
	}


private:
	void init_from_iv(const InputVector& vec)
	{
		copy_from_iv(vec);
	}

private:
	void copy_from_iv(const InputVector& vec)
	{
		decimal_t mod_reci = 1 / mod(vec);		// no need of checking for division by zero.
		for (InputVector::const_iterator it = vec.begin(); it != vec.end(); ++it)
		{
			rv.insert_element(it->fid, mod_reci * it->fval);
		}
	}
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
