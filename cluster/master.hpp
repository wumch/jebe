
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

namespace jebe {
namespace cluster {

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
		LOG_IF(INFO, Aside::config->loglevel > 0) << "start inputing";
		BaseInput* input = create_input();
		input->prepare();
		input->start();
		Document* doc;
		while ((doc = input->next()) &&
			((Aside::totalDocNum != 0) && (Aside::curDocNum < Aside::totalDocNum)))
		{
			process(doc);
		}
		input->stop();
		LOG_IF(INFO, Aside::config->loglevel > 0) << "finished inputing";
	}

	void process(Document* doc)
	{
		Aside::caler->attachDoc(*doc);
	}

	BaseInput* create_input()
	{
		return new NetInput;
	}
};

}
}
