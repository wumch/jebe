
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
#include "config.hpp"
#include "aside.hpp"
#include "calculater.hpp"
#include "netinput.hpp"

namespace jebe {
namespace rel {

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
		BaseInput* input = create_input();
		input->prepare();
		input->start();
		Document* doc;
		while ((doc = input->next()))
		{
			process(doc);
		}
		input->stop();
	}

	void process(Document* doc)
	{
		CS_SAY("first word of doc: " << doc->words[0].word);
		Aside::caler->attachDoc(*doc);
	}

	BaseInput* create_input()
	{
		return new NetInput;
	}
};

}
}
