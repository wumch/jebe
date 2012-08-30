
#include "worker.hpp"
#include <zmq.hpp>
#include "config.hpp"
#include "bus.hpp"

namespace jebe {
namespace fts {

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
		const Config* const config = Config::getInstance();
		sock.connect(config->internal.c_str());
		sock.setsockopt(ZMQ_SNDBUF, &config->send_buffer_size, sizeof(config->send_buffer_size));
		sock.setsockopt(ZMQ_RCVBUF, &config->receive_buffer_size, sizeof(config->receive_buffer_size));
	}

	while (true)
	{
		recv_buf.rebuild();
		send_buf.rebuild();
		CS_SAY("waiting for incoming");
		sock.recv(&recv_buf, 0);
		CS_SAY("received bytes: " << recv_buf.size());
		bus.route(recv_buf, send_buf);
		CS_SAY("processed, res bytes: " << send_buf.size());
		sock.send(send_buf);
		CS_SAY("sent");
	}
}

} /* namespace fts */
} /* namespace jebe */
