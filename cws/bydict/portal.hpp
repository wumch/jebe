
#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/asio/signal_set.hpp>
#include "predef.hpp"
#include "master.hpp"
#include "shareinfo.hpp"

namespace jebe {
namespace cws {

class Portal
{
public:
    explicit Portal(const char* config_file);

    void handle_signals();

    void run();

    void stop();

    ~Portal();

private:
    void signals_init();

    void store_pid();

    static void clean_pid();

    static void terminate();

    boost::asio::signal_set* signals;

    Master* master;

    boost::shared_ptr<BA::ip::tcp::endpoint> ep;
};

}
}
