
#pragma once

#include "predef.hpp"
#include <Eigen/Sparse>
#include "vector.hpp"
#include "hash.hpp"
#include "math.hpp"

namespace jebe {
namespace classify {
namespace rknn {

// cosine() for two normalized <Vector>.
static inline decimal_t cos(const Vector& v1, const Vector& v2)
{
	return v1->dot(*v2);
}

// similarity of two normalized <Vector>.
static inline decimal_t similarity(const Vector& v1, const Vector& v2)
{
	return cos(v1, v2);
}

// disparity of two normalized <Vector>.
static inline decimal_t disparity(const Vector& v1, const Vector& v2)
{
	return 1 - cos(v1, v2);
}

// modulo of a <InputVector> (no need of nomalized).
static inline decimal_t mod(const InputVector& vec)
{
	decimal_t square_sum = .0;
	for (FeatureList::const_iterator it = vec->begin(); it != vec->end(); ++it)
	{
		square_sum += staging::square(it->fval);
	}
	return std::sqrt(square_sum);
}

// modulo of a <Vector> (no need of nomalized).
static inline decimal_t mod(const RawVector& vec)
{
	return std::sqrt(vec.squaredNorm());
}

static inline void normalize(RawVector& vec)
{
	vec /= mod(vec);
}

static inline void normalize_lazy(RawVector& vec)
{
	vec /= mod(vec);
}

}
}
}
