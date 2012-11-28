
#include "vector.hpp"
#include <cmath>
#include "math.hpp"
#include "vector_impl.hpp"
#include "../document.hpp"

namespace jebe {
namespace cluster {
namespace kmeans {

class FeatureComparer
{
public:
	bool operator()(const Feature& f1, const Feature& f2) const
	{
		return f1.fid < f2.fid;
	}
};

Vector::Vector()
	: rv(Aside::totalFeatureNum, Aside::config->reserve_fnum)
{}

Vector::Vector(const Document& doc)
	: VectorMeta(doc.id), rv(Aside::totalFeatureNum, doc.flist.size())
{
	init_from_iv(doc.flist);
}

Vector::Vector(vid_t id_, const RawVector& rv_)
	: VectorMeta(id_), rv(rv_)
{}

Vector::Vector(const Vector& vec)
	: VectorMeta(vec.id, vec.belong_cls), rv(vec.rv.size(), vec.rv.nnz())
{
	rv.assign(vec.rv);
}

void Vector::reset(const RawVector& rv_)
{
	rv = rv_;
}

Vector::operator RawVector()
{
	return rv;
}

void Vector::init_from_iv(const InputVector& vec)
{
	copy_from_iv(vec);
}

void Vector::copy_from_iv(const InputVector& vec)
{
	decimal_t mod_reci = 1 / mod(vec);		// no need of checking for division by zero.
	for (InputVector::const_iterator it = vec.begin(); it != vec.end(); ++it)
	{
		rv.insert_element(it->fid, mod_reci * it->fval);
	}
}

// work around boost::mapped_vector.
RawVector& Vector::operator*()
{
	return rv;
}

const RawVector& Vector::operator*() const
{
	return rv;
}

RawVector* Vector::operator->()
{
	return &rv;
}

const RawVector* Vector::operator->() const
{
	return &rv;
}

} /* namespace kmeans */
} /* namespace cluster */
} /* namespace jebe */
