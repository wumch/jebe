
/**
 * matio
 * just a lib, not stand-alone execute-able.
 */

#include "../predef.hpp"
#include <boost/static_assert.hpp>

namespace jebe {
namespace dsvd {
namespace matio {

typedef size_t size_type;
typedef double scalar_type;
BOOST_STATIC_ASSERT(sizeof(size_type) >= 4 && sizeof(scalar_type) >= 4);

}
}
}
