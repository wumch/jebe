
#include "master.hpp"
#include <vector>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#ifdef __linux
#	include <sys/prctl.h>
#endif
#include "staging/net.hpp"

namespace jebe {
namespace policy {

Master::Master(std::size_t worker_count_, const std::string& response_)
    : worker_count(worker_count_), response(response_), respBuffer(response.data(), response.size())
{
	CS_SAY("construct master");
	CS_SAY("response-data: " << response);
}

void Master::run()
{
#ifdef __linux
	prctl(PR_SET_NAME, "policy");
#endif
	io_service = new boost::asio::io_service;
	new boost::asio::io_service::work(*io_service);
    for (std::size_t i = 0; i < worker_count; ++i)
    {
        boost::shared_ptr<boost::thread> thread(new boost::thread(boost::bind(&boost::asio::io_service::run, io_service)));
        threads.push_back(thread);
    }

    listen();
    start_accept();
    io_service->run();
    for (ThreadList::iterator iter = threads.begin(); iter != threads.end(); ++iter)
    {
        (*iter)->join();
    }
}

void Master::stop()
{
	io_service->stop();
}

void Master::listen()
{
	namespace ip = boost::asio::ip;

    acptor = new ip::tcp::acceptor(getio());
    
    ip::tcp::resolver resolver(acptor->get_io_service());
    ip::tcp::resolver::query query(staging::getWanIP(), "843");
    ip::tcp::endpoint ep = *resolver.resolve(query);
    acptor->open(ep.protocol());

    acptor->set_option(ip::tcp::acceptor::reuse_address(true));
    acptor->set_option(ip::tcp::acceptor::send_buffer_size(response.size()));
    acptor->set_option(ip::tcp::acceptor::receive_buffer_size(0));
    
    acptor->bind(ep);
    acptor->listen(1024);
}

void Master::start_accept()
{
	SockType* s = alloc_session();
	if (CS_LIKELY(s))
	{
		SockPtr sess(s, boost::bind(&Master::release_session, this, s));
		acptor->async_accept(*s,
			boost::bind(&Master::handle_accept, this,
				sess, boost::asio::placeholders::error
			)
		);
	}
	else
	{
		delay_accept();
	}
}

void Master::delay_accept()
{
	CS_SAY("connections: " << sess_count);
	static const std::size_t max_interval = 200;
	static std::size_t cur_interval = 2;
	static boost::asio::deadline_timer timer(getio());

	timer.expires_from_now(boost::posix_time::millisec(cur_interval));
	timer.async_wait(boost::bind(&Master::start_accept, this));

	if (cur_interval < max_interval)
	{
		cur_interval <<= 1;
		if (cur_interval > max_interval)
		{
			cur_interval = max_interval;
		}
	}
}

boost::asio::io_service& Master::getio()
{
	return *io_service;
}

void Master::handle_accept(SockPtr& sock,
    const boost::system::error_code& err_code)
{
    if (!err_code)
    {
    	boost::asio::async_write(*(sock.get()), respBuffer,
			boost::bind(&Master::destroy_session, this, sock)
		);
    }
    start_accept();
}

void Master::runio(boost::asio::io_service& io)
{
#ifdef __linux
	prctl(PR_SET_NAME, "policy");
#endif
    io.run();
}

}
}
