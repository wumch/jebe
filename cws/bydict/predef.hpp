
#pragma once

#include "staging.hpp"
#include <boost/asio.hpp>
#if defined(USE_WCHAR) && USE_WCHAR
extern "C" {
#	include <wchar.h>
}
#endif

#ifndef HAS_LUAPLUS
#	define HAS_LUAPLUS 0
#endif

#define USE_POOL 0
#if defined(USE_POOL) && USE_POOL
#	include "allocator.hpp"
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

#if defined(USE_WCHAR) && USE_WCHAR
typedef wchar_t Atom;
typedef std::wstring Word;
#else
typedef unsigned char Atom;
typedef std::basic_string<Atom> Word;
#endif

typedef Atom* AtomList;
typedef Word Content;
typedef uint32_t ContentLen;
typedef uint8_t WordLen;
typedef int32_t Cursor;
typedef uint32_t atimes_t;

}
}
