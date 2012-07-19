
#pragma once

#include <vector>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

#include "predef.hpp"
#include "singleton.hpp"
#include "worker.hpp"
#include "session.hpp"

namespace jebe {
namespace cws {

class Master
{
	typedef boost::shared_ptr<Session> SessPtr;
public:
    explicit Master(const std::size_t worker_count);

    void run();

    void stop();

    boost::asio::io_service& get_io();

private:
    void listen();

    void start_accept();

    void run_async_io();

    void handle_accept(SessPtr& sock, const boost::system::error_code& error);

    std::size_t pick_worker();

    boost::asio::ip::tcp::acceptor* acptor;

    // hold informations for manage workers
    std::size_t next_worker_index;
    std::size_t worker_count;
    typedef std::vector<boost::shared_ptr<Worker> > WorkerPool;
    WorkerPool workers;
};

}
}
