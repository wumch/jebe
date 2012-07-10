
#pragma once

#include <vector>
#include <boost/asio.hpp>
#include "predef.hpp"
#include "config.hpp"

namespace jebe {
namespace cws {

// the global container
namespace G {

#define HTTP_LINE_SEP "\r\n"
#define HTTP_SEP HTTP_LINE_SEP HTTP_LINE_SEP
#define HTTP_CONTENT_LENGTH "Content-Length: "

#define HEADER_NAME(status) header_##status
#define HEADER(status) G::HEADER_NAME(status)
#define HEADER_STRING(status) "HTTP/1.0 "#status" OK"HTTP_LINE_SEP"Content-Type: text/plain; charset=utf-8"HTTP_LINE_SEP HTTP_CONTENT_LENGTH
#define MAKE_HEADER(status) std::string HEADER_NAME(status)(HEADER_STRING(status))
#define DECLARE_HEADER(status) extern std::string HEADER_NAME(status)

DECLARE_HEADER(200);
DECLARE_HEADER(400);
extern const std::string httpsep;

// simple message queue for migrate a session from master to worker
typedef std::vector<SessId> SidList;
typedef std::vector<SidList> SidListMap;
extern SidListMap sids_map;

typedef std::vector<pthread_t> WorkerIdList;
extern WorkerIdList worker_id_list;

// the io_service for master
extern BA::io_service mio;

#ifndef SESSION_READY
#define SESSION_READY "session_ready"
#endif
// the nitification for session ready
extern const char session_ready[sizeof(SESSION_READY)];

}
}
}
