
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <boost/lexical_cast.hpp>
#include "portal.hpp"

namespace jebe {
namespace ics {

Portal::Portal()
{
    master = new Master(Config::getInstance()->worker_count);
//    signals = new boost::asio::signal_set(master->get_io());
    std::set_terminate(Portal::terminate);
}

void Portal::signals_init()
{
//    signals->add(SIGHUP);
//    signals->async_wait(boost::bind(&Portal::handle_signals, this));
}

void Portal::handle_signals()
{
    CS_SAY("will shoutdown after all works done");
    stop();
}

void Portal::stop()
{
    master->stop();;
    clean_pid();
}

void Portal::run()
{
    store_pid();
//    signals_init();
    master->run();
    clean_pid();
}

void Portal::store_pid()
{
    std::ofstream pidfile(Config::getInstance()->pidfile.string().c_str());
    if (!pidfile)
    {
        std::cerr << "faild on create pifile" << std::endl;
        exit(1);
    }
    pidfile << getpid() << std::flush;
}

void Portal::clean_pid()
{
    std::ifstream pidfin(Config::getInstance()->pidfile.string().c_str());
    if (!pidfin)
    {
        std::cerr << "faild on clean pifile" << std::endl;
        exit(1);
    }
    std::string pid;
    pidfin >> pid;
    pidfin.close();
    
    if (boost::lexical_cast<int>(pid) == getpid())
    {
        std::ofstream pidfout(Config::getInstance()->pidfile.string().c_str());
        pidfout.close();
    }
}

void Portal::terminate()
{
    clean_pid();
    std::terminate();
}

Portal::~Portal()
{
    clean_pid();
}

}
}
