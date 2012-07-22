
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
#include "singleton.hpp"
#include "unit.hpp"
#include "worker.hpp"
#include "session.hpp"

namespace jebe {
namespace cws {

class Master
{
	typedef std::vector<boost::shared_ptr<boost::thread> > ThreadList;
	typedef boost::shared_ptr<Session> SessPtr;
public:
    explicit Master(const std::size_t worker_count);

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

    void handle_accept(SessPtr& sock, const boost::system::error_code& error);

    void release_session(Session* sess)
    {
    	sess->release();
    	SessAlloc::free(sess);
    	--sess_count;
    }

    Session* alloc_session()
    {
    	if (CS_BLIKELY(sess_count < Config::getInstance()->max_connections))
    	{
			Session* sess = new (SessAlloc::malloc()) Session(getio());
//			Session* sess = new Session(getio());
			if (CS_LIKELY(sess))
			{
				++sess_count;
				return sess;
			}
    	}
    	return NULL;
    }

    boost::asio::ip::tcp::acceptor* acptor;

    // hold informations for manage workers
    std::size_t next_io;
    std::size_t worker_count;

    typedef std::vector<boost::asio::io_service*> IoPool;
    IoPool ios;
    boost::asio::io_service* io_service;

    typedef std::vector<boost::asio::io_service::work*> WorkPool;
    WorkPool works;

    typedef boost::singleton_pool<Session, sizeof(Session), boost::default_user_allocator_new_delete,
    		boost::details::pool::default_mutex, _JEBE_SESS_POOL_INC_STEP> SessAlloc;

    std::size_t sess_count;
};

}
}
