
#pragma once

#include "staging.hpp"
#include <boost/asio.hpp>

#ifndef HAS_LUAPLUS
#	define HAS_LUAPLUS 0
#endif

#define USE_POOL 0
#if defined(USE_POOL) && USE_POOL
#include "allocator.hpp"
#endif

namespace jebe {
namespace cws {

namespace BA = boost::asio;
namespace BS = boost::system;

typedef enum {
    SUCCESS = 1, FAILD
} rescode;

typedef std::size_t SessId;

typedef BA::ip::tcp::socket Sock;
typedef boost::shared_ptr<Sock> SockPtr;

typedef uint8_t SceneId;

typedef wchar_t CharType;

}
}
