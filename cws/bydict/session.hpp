
#pragma once

#include "predef.hpp"
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/assert.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/array.hpp>

#include "singleton.hpp"
#include "filter.hpp"

#define _JEBE_HTTP_LINE_SEP "\r\n"
#define _JEBE_HTTP_SEP _JEBE_HTTP_LINE_SEP _JEBE_HTTP_LINE_SEP
#define _JEBE_HTTP_CONTENT_LENGTH "Content-Length: "

#define _JEBE_HEADER_NAME(status) BOOST_PP_CAT(header_, status)
#define _JEBE_HEADER(status) _JEBE_HEADER_NAME(status)
#define _JEBE_HEADER_STRING(status) "HTTP/1.0 "#status" OK"_JEBE_HTTP_LINE_SEP"Content-Type: text/plain; charset=utf-8"_JEBE_HTTP_LINE_SEP _JEBE_HTTP_CONTENT_LENGTH
#define _JEBE_MAKE_HEADER(status, klass)	const std::string klass::_JEBE_HEADER_NAME(status)(_JEBE_HEADER_STRING(status))
#define _JEBE_DECLARE_HEADER(status)		static const std::string _JEBE_HEADER_NAME(status)

namespace jebe {
namespace cws {

extern const Filter* filter;

class Session:
    public boost::enable_shared_from_this<Session>,
    public boost::noncopyable
{
public:
    typedef boost::asio::mutable_buffers_1 ReadBuf;

    explicit Session(boost::asio::io_service& io, std::string& request_, byte_t* const res_, std::string& response_)
        : sock(io),
        request(request_), res(res_), response(response_),
        transferred(0), write_times(0)
#if _JEBE_USE_TIMER
    	,timer(io)
#endif
    {
    }

    ~Session()
    {
    	CS_SAY("session destroyed");
    }

    boost::asio::ip::tcp::socket& getSock()
    {
        return sock;
    }

    void inline start();

    static void config();

protected:
    static const std::string httpsep;
    _JEBE_DECLARE_HEADER(200);
    _JEBE_DECLARE_HEADER(400);

    static std::size_t body_max_len;
    static std::size_t header_max_len;
    static std::size_t max_len;
    static std::size_t timeout;
    static std::size_t max_write_times;
    
protected:
    void inline start_receive();

    void start_receive(const std::size_t offset);

    void handle_read(const boost::system::error_code& error,
        const std::size_t bytes_transferred);

    void inline finish(const boost::system::error_code& error = boost::system::error_code());

    void finish_by_wait(const boost::system::error_code& error = boost::system::error_code());

    void inline reply();

    boost::asio::ip::tcp::socket sock;

    std::string& request;
    byte_t* const res;
    std::string& response;
    std::size_t transferred;
    std::size_t write_times;
#if _JEBE_USE_TIMER
    boost::asio::deadline_timer timer;
#endif
};

}
}
