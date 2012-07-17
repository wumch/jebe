
#pragma once

#ifdef __linux
#	include <sys/prctl.h>
#	include <unistd.h>
#	include <sys/syscall.h>
#	include <pthread.h>
#endif

#include <boost/asio/io_service.hpp>
#include <boost/thread.hpp>
#include "predef.hpp"
#include "session.hpp"
#include "shareinfo.hpp"

namespace jebe {
namespace cws {

class Worker
{
public:
    typedef BA::ip::tcp::socket Sock;

    explicit Worker()
        : io(*(new BA::io_service())), work(*(new BA::io_service::work(io)))
    {
    }

    explicit Worker(Worker& worker)
        : io(worker.io), work(worker.work)
    {
    }

    explicit Worker(const Worker& worker)
        : io(worker.io), work(worker.work)
    {
    }

    void run()
    {
#ifdef __linux
    	prctl(PR_SET_NAME, (G::config->program_name + ":worker").c_str());
#endif
        io.run();
    }

    void stop()
    {
        io.stop();
    }

    BA::io_service& get_io() const
    {
        return io;
    }

protected:
    BA::io_service& io;

    BA::io_service::work& work;
};

}
}
