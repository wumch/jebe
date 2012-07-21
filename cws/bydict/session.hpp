
#pragma once

#include "predef.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/assert.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/array.hpp>
#include "unit.hpp"
#include "singleton.hpp"
#include "filter.hpp"

#define _JEBE_HTTP_LINE_SEP 				"\r\n"
#define _JEBE_HTTP_SEP 						_JEBE_HTTP_LINE_SEP _JEBE_HTTP_LINE_SEP
#define _JEBE_HTTP_CONTENT_LENGTH 			"Content-Length: "

#define _JEBE_HEADER_NAME(status)			BOOST_PP_CAT(header_, status)
#define _JEBE_HEADER(status) 				_JEBE_HEADER_NAME(status)
#define _JEBE_HEADER_STRING(status) 		"HTTP/1.0 "#status" OK"_JEBE_HTTP_LINE_SEP"Content-Type: text/plain; charset=utf-8"_JEBE_HTTP_LINE_SEP _JEBE_HTTP_CONTENT_LENGTH
#define _JEBE_MAKE_HEADER(status, klass)	const std::string klass::_JEBE_HEADER_NAME(status)(_JEBE_HEADER_STRING(status))
#define _JEBE_DECLARE_HEADER(status)		static const std::string _JEBE_HEADER_NAME(status)

namespace jebe {
namespace cws {

enum Ation {
	split,
	count,
	compare,
};

extern const Filter* filter;
class Worker;

class Session:
    public boost::enable_shared_from_this<Session>,
    public boost::noncopyable
{
public:
	typedef boost::fast_pool_allocator<staging::CSUnit<1>, boost::default_user_allocator_new_delete,
			boost::details::pool::null_mutex,
			_JEBE_SESS_POOL_INC_STEP * _JEBE_SESS_RBUF_UNIT,
			_JEBE_SESS_POOL_MAX_SIZE * _JEBE_SESS_RBUF_UNIT> RecvBuffAlloc;
//	static RecvBuffAlloc recv_buff_alloc;

	typedef std::basic_string<char, std::char_traits<char>, RecvBuffAlloc> pstr;

	typedef boost::singleton_pool<staging::CSUnit<2>, sizeof(char), boost::default_user_allocator_new_delete,
			boost::details::pool::null_mutex,
			_JEBE_SESS_POOL_INC_STEP * _JEBE_SESS_RBUF_UNIT,
			_JEBE_SESS_POOL_MAX_SIZE * _JEBE_SESS_RBUF_UNIT> ResBuffAlloc;
//	static SendBuffAlloc res_buff_alloc;

	typedef boost::fast_pool_allocator<staging::CSUnit<2>, boost::default_user_allocator_new_delete,
			boost::details::pool::null_mutex,
			_JEBE_SESS_POOL_INC_STEP * _JEBE_SESS_RBUF_UNIT,
			_JEBE_SESS_POOL_MAX_SIZE * _JEBE_SESS_RBUF_UNIT> SendBuffAlloc;
//	static SendBuffAlloc send_buff_alloc;

    typedef boost::asio::mutable_buffers_1 ReadBuf;

//    explicit Session(boost::asio::io_service& io, pstr& request_, byte_t* const res_, pstr& response_)
//        : sock(io),
//        request(request_), res(res_), response(response_),
//        transferred(0), write_times(0)
//#if _JEBE_USE_TIMER
//    	,timer(io)
//#endif
//    {
//    }

    explicit Session(boost::asio::io_service& io);

    ~Session()
    {
//		request.assign(request.size(), 0);
//		SendBuffAlloc::free(const_cast<char*>(request.data()), Config::getInstance()->receive_buffer_size);
    	ResBuffAlloc::ordered_free(res, Config::getInstance()->send_buffer_size);
//		SendBuffAlloc::free(const_cast<char*>(response.data()), Config::getInstance()->send_buffer_size);
//		response.assign(response.size(), 0);
//		memset(res, 0, Config::getInstance()->body_max_size << 1);
    }

    boost::asio::ip::tcp::socket& getSock()
    {
        return sock;
    }

    void inline start();

    static void config();

protected:
    static const std::string httpsep;
    static const std::string bodysep;
    static const std::string contentLength;
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

    void finish(const boost::system::error_code& error = boost::system::error_code())
    {
        if (!error)
        {
#if _JEBE_USE_TIMER
            timer.cancel();
#endif
            boost::system::error_code ignored_ec;
            sock.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
        }
    }

    void finish_by_wait(const boost::system::error_code& error = boost::system::error_code());

    void inline reply();

    boost::asio::ip::tcp::socket sock;

    std::size_t transferred;
    std::size_t write_times;
    char* request;
    byte_t* const res;
    byte_t* const response;
#if _JEBE_USE_TIMER
    boost::asio::deadline_timer timer;
#endif
};

    void Session::start()
    {
        start_receive();
#if _JEBE_USE_TIMER
        timer.expires_from_now(boost::posix_time::millisec(timeout));
        timer.async_wait(
            boost::bind(&Session::finish_by_wait, shared_from_this(), boost::asio::placeholders::error)
        );
#endif
    }

    void Session::start_receive()
    {
        sock.async_read_some(
            boost::asio::buffer(const_cast<char*>(request.data()), max_len),
            boost::bind(&Session::handle_read, shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred
            )
        );
    }

//    void Session::finish(const boost::system::error_code& error);

    void Session::reply()
    {
        boost::asio::async_write(sock,
            boost::asio::buffer(response, response.size()),
            boost::bind(&Session::finish, shared_from_this(),
                boost::asio::placeholders::error
            )
        );
    }


}
}
