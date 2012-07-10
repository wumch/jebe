
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <boost/lexical_cast.hpp>
#include "portal.hpp"

namespace jebe {
namespace cws {

Portal::Portal(const char* config_file)
{
    G::config_file = config_file;
    G::config = Config::getInstance();
    master = new Master(Config::getInstance()->worker_cout);
    signals = new BA::signal_set(master->get_io());
    std::set_terminate(Portal::terminate);
}

void Portal::signals_init()
{
    signals->add(SIGHUP);
    signals->async_wait(boost::bind(&Portal::handle_signals, this));
}

void Portal::handle_signals()
{
    std::cout << "will shoutdown after all works done" << std::endl;
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
    signals_init();
    master->run();
    clean_pid();
}

void Portal::store_pid()
{
    std::ofstream pidfile(Config::getInstance()->pidfile.c_str());
    if (!pidfile)
    {
        std::cerr << "faild on create pifile" << std::endl;
        exit(1);
    }
    pidfile << getpid() << std::flush;
}

void Portal::clean_pid()
{
    std::ifstream pidfin(Config::getInstance()->pidfile.c_str());
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
        std::ofstream pidfout(Config::getInstance()->pidfile.c_str());
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
