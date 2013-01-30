
#include "inputvector.hpp"

namespace jebe {
namespace classify {

size_t InputVector::moffset = 0;
msgpack::object InputVector::mobj;
msgpack::zone InputVector::mzone(1 << 20);

} /* namespace classify */
} /* namespace jebe */
