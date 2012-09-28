
#pragma once

#include "predef.hpp"
#include <new>
#include <vector>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/pool/pool_alloc.hpp>

namespace jebe {
namespace policy {

class Master
{
    typedef boost::asio::ip::tcp::socket SockType;
    typedef boost::singleton_pool<SockType, sizeof(SockType), boost::default_user_allocator_new_delete,
    		boost::details::pool::default_mutex, 256> SessAlloc;

	typedef std::vector<boost::shared_ptr<boost::thread> > ThreadList;
	typedef boost::shared_ptr<boost::asio::ip::tcp::socket> SockPtr;

public:
    explicit Master(std::size_t worker_count, const std::string& response_);

    void run();

    void stop();

    boost::asio::io_service& getio();

    ~Master()
    {
    	CS_SAY("destruct master");
    }

protected:
    void listen();

    void start_accept();

    void delay_accept();

    void runio(boost::asio::io_service& io);

    void handle_accept(SockPtr& sock, const boost::system::error_code& error);

    void release_session(SockType* sock)
    {
    	if (CS_LIKELY(sock))
    	{
			CS_SAY("release session");
//			sock->cancel();
			boost::system::error_code ignored_ec;
			sock->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
//			SessAlloc::free(sock);
			delete sock;
			--sess_count;
    	}
    }

    void destroy_session(SockPtr& sock)
    {

    }

    SockType* alloc_session()
    {
    	if (CS_BLIKELY(sess_count < 8192))
    	{
//    		SockType* sock = new (SessAlloc::malloc()) boost::asio::ip::tcp::socket(getio());
    		SockType* sock = new boost::asio::ip::tcp::socket(getio());
			if (CS_LIKELY(sock))
			{
				++sess_count;
				return sock;
			}
    	}
    	return NULL;
    }

    boost::asio::io_service* io_service;
    boost::asio::ip::tcp::acceptor* acptor;

    ThreadList threads;
    std::size_t worker_count;

    std::size_t sess_count;

    const std::string& response;
    boost::asio::const_buffers_1 respBuffer;
};

}
}
