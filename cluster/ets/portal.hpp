
#pragma once

#include <boost/shared_ptr.hpp>
#include "predef.hpp"
#include "master.hpp"

namespace jebe {
namespace cluster {
namespace ets {

class Portal
{
public:
    explicit Portal();

    void handle_signals();

    void run();

    void stop();

    ~Portal();

private:
    void signals_init();

    void store_pid();

    static void clean_pid();

    static void terminate();

//    boost::asio::signal_set* signals;

    Master* master;

    boost::shared_ptr<boost::asio::ip::tcp::endpoint> ep;
};

} /* namespace ets */
} /* namespace cluster */
} /* namespace jebe */
