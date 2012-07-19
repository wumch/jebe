
#include <vector>
#include <boost/thread.hpp>
#ifdef __linux
#	include <sys/prctl.h>
#endif
#include "master.hpp"
#include "config.hpp"

namespace jebe {
namespace cws {

typedef std::vector<boost::shared_ptr<boost::thread> > ThreadList;

Master::Master(const std::size_t worker_count)
    : next_worker_index(0), workers(worker_count)
{
    for (WorkerPool::iterator iter = workers.begin(); iter != workers.end(); ++iter)
    {
        iter->reset(new Worker);
    }
}

void Master::run()
{
#ifdef __linux
	prctl(PR_SET_NAME, (Config::getInstance()->program_name + ":master").c_str());
#endif
    ThreadList threads;
    for (WorkerPool::iterator iter = workers.begin(); iter != workers.end(); ++iter)
    {
        iter->reset(new Worker);
        boost::shared_ptr<boost::thread> thread(new boost::thread(boost::bind(&Worker::run, *iter)));
        threads.push_back(thread);
    }

    filter = new Filter(Config::getInstance()->patten_file.string());
    
    listen();
    for (ThreadList::iterator iter = threads.begin(); iter != threads.end(); ++iter)
    {
        (*iter)->join();
    }
}

void Master::stop()
{
    for (WorkerPool::iterator iter = workers.begin(); iter != workers.end(); ++iter)
    {
        (*iter)->stop();
    }
}

void Master::listen()
{
    acptor = new boost::asio::ip::tcp::acceptor(get_io());
    const Config* const config = Config::getInstance();
    
    boost::asio::ip::tcp::resolver resolver(acptor->get_io_service());
    boost::asio::ip::tcp::resolver::query query(config->host, boost::lexical_cast<std::string>(config->port));
    boost::asio::ip::tcp::endpoint ep = *resolver.resolve(query);
    acptor->open(ep.protocol());

    acptor->set_option(boost::asio::ip::tcp::acceptor::reuse_address(config->reuse_address));
    acptor->set_option(boost::asio::ip::tcp::acceptor::send_buffer_size(config->send_buffer_size));
    acptor->set_option(boost::asio::ip::tcp::acceptor::receive_buffer_size(config->receive_buffer_size));

    Session::config();
    
    acptor->bind(ep);
    acptor->listen();
    start_accept();
}

void Master::start_accept()
{
	Worker& w = *(workers[pick_worker()]);
    SessPtr sess(new Session(w.get_io(), w.request, w.res, w.response));
    acptor->async_accept(sess->getSock(),
        boost::bind(&Master::handle_accept, this,
            sess, boost::asio::placeholders::error
        )
    );
}

boost::asio::io_service& Master::get_io()
{
    return workers[pick_worker()]->get_io();
}

std::size_t Master::pick_worker()
{
    ++next_worker_index;
    if (next_worker_index == workers.size())
    {
        next_worker_index = 0;
    }
    return next_worker_index;
}

void Master::handle_accept(SessPtr& sess,
    const boost::system::error_code& err_code)
{
    if (!err_code)
    {
        sess->start();
    }
    start_accept();
}

}
}
