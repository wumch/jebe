
#pragma once

#include "predef.hpp"
#include <string>
#include "config.hpp"

namespace jebe {
namespace cws {

#define _JEBE_HTTP_LINE_SEP "\r\n"
#define _JEBE_HTTP_SEP _JEBE_HTTP_LINE_SEP _JEBE_HTTP_LINE_SEP
#define _JEBE_HTTP_CONTENT_LENGTH "Content-Length: "

#define _JEBE_HEADER_NAME(status) header_##status
#define _JEBE_HEADER(status) _JEBE_HEADER_NAME(status)
#define _JEBE_HEADER_STRING(status) "HTTP/1.0 "#status" OK"_JEBE_HTTP_LINE_SEP"Content-Type: text/plain; charset=utf-8"_JEBE_HTTP_LINE_SEP _JEBE_HTTP_CONTENT_LENGTH
#define _JEBE_MAKE_HEADER(status) std::string _JEBE_HEADER_NAME(status)(_JEBE_HEADER_STRING(status))
#define _JEBE_DECLARE_HEADER(status) extern std::string _JEBE_HEADER_NAME(status)

_JEBE_DECLARE_HEADER(200);
_JEBE_DECLARE_HEADER(400);
extern const std::string httpsep;

}
}
