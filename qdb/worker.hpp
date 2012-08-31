
#pragma once

#include "predef.hpp"
#include <zmq.hpp>
#include "bus.hpp"

namespace jebe {
namespace qdb {

class Worker
{
public:
	Worker(zmq::context_t& context);

	void run();

	void stop();

private:
	zmq::context_t& context;

	zmq::message_t recv_buf;

	zmq::message_t send_buf;

	Bus bus;
};

} /* namespace qdb */
} /* namespace jebe */
