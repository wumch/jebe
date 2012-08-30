
#pragma once

#include "predef.hpp"
#include <vector>
#include <iostream>
#include <boost/thread.hpp>
#ifdef __linux
#	include <sys/prctl.h>
#endif
#include <zmq.hpp>
#include "config.hpp"
#include "worker.hpp"

namespace jebe {
namespace fts {

class Master
{
public:
	Master()
		: context(Config::getInstance()->io_threads)
	{
	}

	void run()
	{
		prety();

		boost::thread* router_thread = new boost::thread(boost::bind(&Master::start_router, this));

		for (std::size_t i = 0; i < Config::getInstance()->worker_count; ++i)
		{
			Worker* const worker = new Worker(context);
			boost::shared_ptr<boost::thread> thread(new boost::thread(boost::bind(&Worker::run, worker)));
			threads.push_back(thread);
		}

		router_thread->join();
		for (ThreadList::iterator it = threads.begin(); it != threads.end(); ++it)
		{
			(*it)->join();
		}
	}

	void _run()
	{
		prety();
		start_router();
		start_worker();
	}

protected:
	void prety() const
	{
#ifdef __linux
		prctl(PR_SET_NAME, (Config::getInstance()->program_name + ":master").c_str());
#endif
	}

	void start_worker()
	{
		for (std::size_t i = 0; i < Config::getInstance()->worker_count; ++i)
		{
			Worker* const worker = new Worker(context);
			boost::shared_ptr<boost::thread> thread(new boost::thread(boost::bind(&Worker::run, worker)));
			threads.push_back(thread);
			thread->join();
		}
	}

	void start_router()
	{
		zmq::socket_t router(context, ZMQ_ROUTER);
		zmq::socket_t dealer(context, ZMQ_DEALER);

		try
		{
			const Config* const config = Config::getInstance();
			router.bind(config->listen.c_str());
			dealer.bind(config->internal.c_str());
			CS_SAY("binded");
		}
		catch (const zmq::error_t& e)
		{
			CS_DIE("error occured while startup zeromq: " << e.what());
		}
		catch (const std::exception& e)
		{
			CS_DIE("error occured: " << e.what());
		}

		zmq::device(ZMQ_QUEUE, router, dealer);
	}

private:
	typedef std::vector<boost::shared_ptr<boost::thread> > ThreadList;
	ThreadList threads;

	zmq::context_t context;
};

}
}
