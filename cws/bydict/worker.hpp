
#pragma once

#include "predef.hpp"
#include "session.hpp"
#ifdef __linux
#	include <sys/prctl.h>
#	include <unistd.h>
#	include <sys/syscall.h>
#	include <pthread.h>
#endif
#include <boost/asio/io_service.hpp>
#include <boost/thread.hpp>

namespace jebe {
namespace cws {

class Worker
{
public:
    explicit Worker()
        : io(*(new boost::asio::io_service())), work(*(new boost::asio::io_service::work(io))), busy(false),
          request(Config::getInstance()->request_max_size, 0),
          response(Config::getInstance()->request_max_size * 2, 0),
          res(new byte_t[Config::getInstance()->body_max_size << 1])
    {
    }

    void run()
    {
#ifdef __linux
    	prctl(PR_SET_NAME, (Config::getInstance()->program_name + ":worker").c_str());
#endif
        io.run();
    }

    void stop()
    {
        io.stop();
    }

    boost::asio::io_service& get_io()
    {
        return io;
    }

protected:
    boost::asio::io_service& io;

    boost::asio::io_service::work& work;

public:
    bool busy;

    std::string request;

    std::string response;

    byte_t* const res;
};

}
}
