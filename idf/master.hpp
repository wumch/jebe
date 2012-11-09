
#pragma once

#include "predef.hpp"
#include <vector>
#include <iostream>
#include <memory>
#include <boost/thread.hpp>
#ifdef __linux
#	include <sys/prctl.h>
#	include <unistd.h>
#endif
#include <zmq.hpp>
#include <glog/logging.h>
#include "config.hpp"
#include "aside.hpp"
#include "calculater.hpp"
#include "collector.hpp"

namespace jebe {
namespace idf {

class Master
{
public:
	Master()
	{
	}

	void run()
	{
		init();
		prety();
		work();
	}

protected:
	void init() const
	{
		Aside::init();
	}

	void prety() const
	{
#ifdef __linux
		prctl(PR_SET_NAME, (Aside::config->program_name + ":master").c_str());
#endif
	}

	void work()
	{
		boost::shared_ptr<zmq::context_t> context(new zmq::context_t(Aside::config->io_threads));

		for (uint i = 0; i < Aside::config->calculater_num; ++i)
		{
			boost::shared_ptr<Calculater> calculater(new Calculater(*context, i));
			threads.push_back(new boost::thread(boost::bind(&Calculater::run, calculater)));
		}

		for (uint i = 0; i < Aside::config->collector_num; ++i)
		{
			boost::shared_ptr<Collector> collector(new Collector(*context));
			threads.push_back(new boost::thread(boost::bind(&Collector::run, collector)));
		}

		for (Threads::iterator it = threads.begin(); it != threads.end(); ++it)
		{
			(*it)->join();
		}
	}

private:
	typedef std::vector<boost::thread*> Threads;
	Threads threads;
};

}
}
