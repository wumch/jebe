
#pragma once

#include "predef.hpp"
#include <vector>
#include <iostream>
#include <boost/thread.hpp>
#ifdef __linux
#	include <sys/prctl.h>
#	include <unistd.h>
#	include <signal.h>
#endif
#include <zmq.hpp>
#include "config.hpp"
#include "worker.hpp"
#include "storage.hpp"

namespace jebe {
namespace qdb {

class Master
{
public:
	Master()
		: context(Config::getInstance()->io_threads)
	{
	}

	void run()
	{
		init();
		prety();
		create_router();
		create_worker();
		run_all();
	}

	~Master()
	{
		stop();
	}

	static void stop()
	{
		delete Storage::getInstance();
	}

protected:
	void init() const
	{
		Storage::getInstance();	// <Storage>.build()
		signal_init();
	}

	void prety() const
	{
#ifdef __linux
		prctl(PR_SET_NAME, (Config::getInstance()->program_name + ":master").c_str());
#endif
	}

	void run_all()
	{
		router->join();
		for (ThreadList::iterator it = threads.begin(); it != threads.end(); ++it)
		{
			(*it)->join();
		}
	}

	void create_worker()
	{
		for (std::size_t i = 0; i < Config::getInstance()->worker_count; ++i)
		{
			Worker* const worker = new Worker(context);
			boost::shared_ptr<boost::thread> thread(new boost::thread(boost::bind(&Worker::run, worker)));
			threads.push_back(thread);
		}
	}

	void create_router()
	{
		router = new boost::thread(boost::bind(&Master::start_router, this));
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

	static void signal_init()
	{
		std::set_terminate(stop);
		signal(SIGTERM, onexit);
		signal(SIGHUP, onexit);
	}

	static void onexit(int sig)
	{
		stop();
		std::exit(0);
	}

private:
	boost::thread* router;

	typedef std::vector<boost::shared_ptr<boost::thread> > ThreadList;
	ThreadList threads;

	zmq::context_t context;
};

}
}
