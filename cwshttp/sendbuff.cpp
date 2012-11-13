
#include "sendbuff.hpp"

namespace jebe {
namespace cwshttp {

tsize_t SendBuff::chunkSize;
tsize_t SendBuff::chunkRate;
uint32_t SendBuff::httpsep = *reinterpret_cast<const uint32_t*>(_JEBE_HTTP_SEP _JEBE_HTTP_SEP);

}
}
