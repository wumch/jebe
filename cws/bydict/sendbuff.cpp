
#include "sendbuff.hpp"

namespace jebe {
namespace cws {

tsize_t SendBuff::chunkSize;
uint32_t SendBuff::httpsep = *reinterpret_cast<const uint32_t*>(_JEBE_HTTP_SEP _JEBE_HTTP_SEP);

}
}
