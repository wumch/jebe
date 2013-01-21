
#include "vector.hpp"
#include "vector_impl.hpp"
#include "../document.hpp"

namespace jebe {
namespace cluster {
namespace kmeans {

void Vector::copy_from_iv(const InputVector& vec)
{
	decimal_t mod_reci = 1 / mod(vec);		// no need of checking for division by zero.
	for (InputVector::const_iterator it = vec.begin(); it != vec.end(); ++it)
	{
		rv.insert_element(it->fid, mod_reci * it->fval);
	}
}
} /* namespace kmeans */
} /* namespace cluster */
} /* namespace jebe */
