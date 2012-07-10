
#pragma once

#include <vector>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

#include "predef.hpp"
#include "../../staging/singleton.hpp"
#include "worker.hpp"
#include "session.hpp"
#include "shareinfo.hpp"

namespace jebe {
namespace cws {

class Master
{
public:
    explicit Master(const std::size_t worker_count);

    void run();

    void stop();

    // migrate an accepted socked to worker
    void migrate_session(const SessId sid, SockPtr& sock);

    // move the session's execution-loop to a worker thread
    void notify_worker(const SessId sid);

    void handle_sessid(SockPtr& sock, const Session::ReadBuf& buf,
        const BS::error_code& err_code, const std::size_t bytes_transfered);

    SessId peek_sess_id(SockPtr& sock, const Session::ReadBuf& buf,
        const std::size_t bytes_transfered) const;

    void test_handle_write(const BS::error_code& error);

    BA::io_service& get_io();

private:
    void listen();

    void start_accept();

    void run_async_io();

    void handle_accept(SessPtr& sock, const BS::error_code& error);

    std::size_t pick_worker();

    BA::ip::tcp::acceptor* acptor;

    // hold informations for manage workers
    std::size_t next_worker_index;
    std::size_t worker_count;
    typedef std::vector<boost::shared_ptr<Worker> > WorkerPool;
    WorkerPool workers;

    SessPtr sess;

    BA::const_buffers_1 buf_sready;
};

}
}
