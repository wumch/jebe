
#pragma once

#include "predef.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/assert.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/find.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/array.hpp>
#include "unit.hpp"
#include "singleton.hpp"
#include "filter.hpp"
#include "sendbuff.hpp"
#include "request_handler.hpp"

#define _JEBE_HTTP_CONTENT_LENGTH 			"Content-Length: "

#define _JEBE_HEADER_NAME(status)			BOOST_PP_CAT(header_, status)
#define _JEBE_HEADER(status) 				_JEBE_HEADER_NAME(status)
#define _JEBE_HEADER_STRING(status) 		"HTTP/1.0 "BOOST_PP_STRINGIZE(status)" OK"_JEBE_HTTP_LINE_SEP"Content-Type: text/plain; charset=utf-8"_JEBE_HTTP_LINE_SEP _JEBE_HTTP_CONTENT_LENGTH
#define _JEBE_MAKE_HEADER(status, klass)	const byte_t klass::_JEBE_HEADER_NAME(status)[] = _JEBE_HEADER_STRING(status)
#define _JEBE_DECLARE_HEADER(status)		static const byte_t _JEBE_HEADER_NAME(status)[CS_CONST_STRLEN(_JEBE_HEADER_STRING(status)) + 1]

namespace jebe {
namespace cws {

class Session:
    public boost::enable_shared_from_this<Session>,
    public boost::noncopyable
{
public:
	typedef boost::singleton_pool<Session, sizeof(char) * _JEBE_SESS_RBUF_UNIT,
			boost::default_user_allocator_new_delete, boost::details::pool::default_mutex,
			_JEBE_SESS_POOL_INC_STEP> RecvBuffAlloc;

    explicit Session(boost::asio::io_service& io);

    boost::asio::ip::tcp::socket& getSock()
    {
        return sock;
    }

    void start()
    {
    	start_receive();
#if _JEBE_USE_TIMER
    	timer.expires_from_now(boost::posix_time::millisec(timeout));
    	timer.async_wait(
    		boost::bind(&Session::finish_by_wait, shared_from_this(), boost::asio::placeholders::error)
    	);
#endif
    }

    static void config();

    void release()
    {
    	handler.~RequestHandler();
    	response.~SendBuff();
    	RecvBuffAlloc::ordered_free(request, chunkRate);
    }

protected:
    static const byte_t httpsep[CS_CONST_STRLEN(_JEBE_HTTP_LINE_SEP) + 1];
    static const byte_t bodysep[CS_CONST_STRLEN(_JEBE_HTTP_SEP) + 1];
    static const byte_t content_length[CS_CONST_STRLEN(_JEBE_HTTP_LINE_SEP _JEBE_HTTP_CONTENT_LENGTH) + 1];
    _JEBE_DECLARE_HEADER(200);
    _JEBE_DECLARE_HEADER(400);

    typedef boost::iterator_range<const byte_t*> Range;
    typedef boost::algorithm::detail::first_finderF<
            boost::range_const_iterator<Range>::type,
			boost::algorithm::is_equal> Matcher;

    static const Matcher httpsep_matcher;
    static const Matcher bodysep_matcher;
    static const Matcher content_length_matcher;
    static const boost::algorithm::is_equal equaler;

    static std::size_t body_max_len;
    static std::size_t header_max_len;
    static std::size_t timeout;
    static std::size_t max_write_times;

    static std::size_t chunkRate;
    static std::size_t chunkSize;

protected:
    void start_receive()
    {
		sock.async_read_some(
			boost::asio::buffer(request, chunkSize),
			boost::bind(&Session::handle_read, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred
			)
		);
    }

    void start_receive(const std::size_t offset);

    void handle_read(const boost::system::error_code& error,
        const std::size_t bytes_transferred);

    void finish(const boost::system::error_code& error)
    {
        if (!error)
        {
#if _JEBE_USE_TIMER
            timer.cancel();
#endif
			boost::system::error_code ignored_ec;
			sock.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
			sock.close();
        }
    }

    void finish()
    {
#if _JEBE_USE_TIMER
		timer.cancel();
#endif
		boost::system::error_code ignored_ec;
		sock.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
		sock.close();
    }

    void finish_by_wait(const boost::system::error_code& error = boost::system::error_code());

    void reply()
    {
    	handler.genRes();
    	boost::asio::async_write(sock,
			response.getBuffers(),
			boost::bind(&Session::finish, shared_from_this(),
				boost::asio::placeholders::error
			)
		);
    }

    boost::asio::ip::tcp::socket sock;

    tsize_t transferred;
    uint16_t write_times;

    tsize_t header_end;
    tsize_t content_length_begin;
    tsize_t body_len;

    byte_t* request;
    SendBuff response;
    RequestHandler handler;

#if _JEBE_USE_TIMER
    boost::asio::deadline_timer timer;
#endif

    ~Session() {}	// prevent from creating <Session> on stack.
};

}
}
