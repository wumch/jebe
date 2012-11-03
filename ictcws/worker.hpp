
#pragma once

#include "predef.hpp"
#include <zmq.hpp>
#include "bus.hpp"

namespace jebe {
namespace ics {

class Worker
{
public:
	Worker(zmq::context_t& context);

	void run();

	void stop();

private:
	void reset()
	{
		memset(send_buf_area, 0, 16);
	}

	zmq::context_t& context;

	char* recv_buf_area;
	zmq::message_t recv_buf;

	char* send_buf_area;
	zmq::message_t send_buf;

	Bus bus;
};

} /* namespace qdb */
} /* namespace jebe */
