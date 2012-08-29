
#include "worker.hpp"
#include <zmq.hpp>
#include "config.hpp"

namespace jebe {
namespace qdb {

Worker::Worker(zmq::context_t& context_)
	: context(context_),
	  recv_buf(Config::getInstance()->receive_buffer_size),
	  send_buf(Config::getInstance()->send_buffer_size)
{
}

void Worker::run()
{
	zmq::socket_t sock(context, ZMQ_REP);
	{
		const Config* config = Config::getInstance();
		sock.setsockopt(ZMQ_SNDBUF, &config->send_buffer_size, sizeof(config->send_buffer_size));
		sock.setsockopt(ZMQ_RCVBUF, &config->receive_buffer_size, sizeof(config->receive_buffer_size));
	}
	while (true)
	{
		sock.recv(&recv_buf, 0);
	}
}

} /* namespace qdb */
} /* namespace jebe */
