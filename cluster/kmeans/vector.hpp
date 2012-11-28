
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
	Vector();

	/*
	 * store only the result of GuiYiHua.
	 * GuiYiHua(vector) = vector / mod(vec).
	 */
	explicit Vector(const Document& doc);

	explicit Vector(vid_t id_, const RawVector& rv_);

	// fix the copy-constructor of boost::mapped_vector.
	Vector(const Vector& vec);

	void reset(const RawVector& rv_);

public:
	CS_FORCE_INLINE RawVector* operator->();
	CS_FORCE_INLINE const RawVector* operator->() const;

	CS_FORCE_INLINE RawVector& operator*();
	CS_FORCE_INLINE const RawVector& operator*() const;

	CS_FORCE_INLINE operator RawVector();

private:
	void init_from_iv(const InputVector& vec);

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
