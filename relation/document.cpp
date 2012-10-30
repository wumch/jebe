
#include "document.hpp"

namespace jebe {
namespace rel {

size_t Document::moffset = 0;
msgpack::object Document::mobj;
msgpack::zone Document::mzone(10 << 20);

} /* namespace rel */
} /* namespace jebe */
