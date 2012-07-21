
#include "predef.hpp"
#include <vector>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#ifdef __linux
#	include <sys/prctl.h>
#endif
#include "config.hpp"
#include "master.hpp"
#include "worker.hpp"

namespace jebe {
namespace cws {

Master::Master(std::size_t worker_count_)
    : next_io(0), worker_count(worker_count_)
{
}

void Master::run()
{
#ifdef __linux
	prctl(PR_SET_NAME, (Config::getInstance()->program_name + ":master").c_str());
#endif
    ThreadList threads;
    for (std::size_t i = 0; i < worker_count; ++i)
    {
    	boost::asio::io_service* io = new boost::asio::io_service;
    	ios.push_back(io);
    	works.push_back(new boost::asio::io_service::work(*io));

        boost::shared_ptr<boost::thread> thread(new boost::thread(boost::bind(&boost::asio::io_service::run, io)));
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
    for (IoPool::iterator iter = ios.begin(); iter != ios.end(); ++iter)
    {
        (*iter)->stop();
    }
}

void Master::listen()
{
	namespace ip = boost::asio::ip;

    acptor = new ip::tcp::acceptor(getio());
    const Config* const config = Config::getInstance();
    
    ip::tcp::resolver resolver(acptor->get_io_service());
    ip::tcp::resolver::query query(config->host, boost::lexical_cast<std::string>(config->port));
    ip::tcp::endpoint ep = *resolver.resolve(query);
    acptor->open(ep.protocol());

    acptor->set_option(ip::tcp::acceptor::reuse_address(config->reuse_address));
    acptor->set_option(ip::tcp::acceptor::send_buffer_size(config->send_buffer_size));
    acptor->set_option(ip::tcp::acceptor::receive_buffer_size(config->receive_buffer_size));

    Session::config();
    
    acptor->bind(ep);
    acptor->listen(Config::getInstance()->max_connections);
    start_accept();
}

//void Master::release_session(Session* sess)
//{
//	sess->~Session();
//}

void Master::start_accept()
{
//	Worker& w = pick_worker();
	Session* s = new (sess_alloc.allocate()) Session(getio());
	SessPtr sess(s, boost::bind(&Master::release_session, this, s));
//	SessPtr sess(new Session(getio()), releaseSession);
	if (CS_LIKELY(sess))
	{
//		SessPtr sess(new Session(getio()));
//    	SessPtr sess(new Session(w.get_io(), w.request, w.res, w.response));
		acptor->async_accept(sess->getSock(),
			boost::bind(&Master::handle_accept, this,
				sess, boost::asio::placeholders::error
			)
		);
	}
	else
	{
		start_accept();
	}
}

boost::asio::io_service& Master::getio()
{
    ++next_io;
    if (next_io == ios.size())
    {
        next_io = 0;
    }
    return *ios[next_io];
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

void Master::runio(boost::asio::io_service& io)
{
#ifdef __linux
	prctl(PR_SET_NAME, (Config::getInstance()->program_name + ":worker").c_str());
#endif
    io.run();
}

}
}
