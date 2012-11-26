
#pragma once

#include "predef.hpp"
#include <boost/numeric/ublas/vector_sparse.hpp>
#include "vector.hpp"
#include "hash.hpp"
#include "math.hpp"
#include "cluster.hpp"

namespace jebe {
namespace cluster {
namespace kmeans {

static inline decimal_t cos(const Vector& v1, const Vector& v2)
{
	return 1 - boost::numeric::ublas::inner_prod(*v1, *v2);
}

static inline decimal_t disparity(const Vector& vec, const Cluster& cls)
{
	return cos(vec, cls.center);
}

static inline decimal_t disparity(const Vector& v1, const Vector& v2)
{
	return cos(v1, v2);
}

static inline decimal_t mod(const InputVector& vec)
{
	decimal_t square_sum = .0;
	for (InputVector::const_iterator it = vec.begin(); it != vec.end(); ++it)
	{
		square_sum += staging::square(it->fval);
	}
	return std::sqrt(square_sum);
}

typedef staging::HashTomasWang32<vid_t> VidHash;

class VectorHash
{
public:
	VidHash hasher;

	vid_t operator()(const vid_t vid)
	{
		return hasher(vid);
	}

	vid_t operator()(const Vector* vec)
	{
		return (*this)(vec->id);
	}

	vid_t operator()(const Vector& vec)
	{
		return (*this)(vec.id);
	}
};

}
}
}
