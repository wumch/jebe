
#include "document.hpp"

namespace jebe {
namespace cluster {

size_t Document::moffset = 0;
msgpack::object Document::mobj;
msgpack::zone Document::mzone(10 << 20);

} /* namespace cluster */
} /* namespace jebe */
