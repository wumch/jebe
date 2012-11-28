
#pragma once

#include "predef.hpp"
#include <vector>
#include <iostream>
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
#include "netinput.hpp"
#include "vector.hpp"
#include "../document.hpp"

namespace jebe {
namespace cluster {
namespace kmeans {

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
		prctl(PR_SET_NAME, (Config::getInstance()->program_name + ":master").c_str());
#endif
	}

	void work()
	{
		LOG_IF(INFO, Aside::config->loglevel > 0) << "waiting for inputing";
		BaseInput* input = create_input();
		input->prepare();
		input->start();
		Document* doc;
		while ((doc = input->next()) &&
			((Aside::totalVecNum == 0) || (Aside::curVecNum < Aside::totalVecNum)))
		{
			process(doc);
		}
		input->stop();
	}

	void process(Document* doc)
	{
		Aside::vecs.push_back(VecFactory::create(*doc));
		__sync_add_and_fetch(&Aside::curVecNum, 1);
	}

	BaseInput* create_input()
	{
		return new NetInput;
	}
};

} /* namespace kmeans */
} /* namespace cluster */
} /* namespace jebe */
