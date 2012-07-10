
#include "shareinfo.hpp"

namespace jebe {
namespace cws {
namespace G {

MAKE_HEADER(200);
MAKE_HEADER(400);

SidListMap sids_map;

WorkerIdList worker_id_list = WorkerIdList();

BA::io_service mio;

const char session_ready[sizeof(SESSION_READY)] = SESSION_READY;

const std::string httpsep(HTTP_SEP);

}
}
}

