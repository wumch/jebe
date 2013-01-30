
#include "worker.hpp"
#ifdef __linux
#	include <unistd.h>
#endif
#include <zmq.hpp>
#include <msgpack.hpp>
#include "config.hpp"
#include "bus.hpp"
#include "aside.hpp"

namespace jebe {
namespace classify {
namespace rknn {

Worker::Worker(zmq::context_t& context_)
	: context(context_),
	  recv_buf_area(new char[Aside::config->receive_buffer_size]),
	  recv_buf(recv_buf_area, Aside::config->receive_buffer_size, NULL, NULL),
	  send_buf_area(new char[Aside::config->send_buffer_size]),
	  send_buf(send_buf_area, Aside::config->send_buffer_size, NULL, NULL)
{
	if (Aside::config->send_buffer_size < 16)
	{
		CS_DIE("send-buffer-size must not less than 16, current: " << Aside::config->send_buffer_size);
	}
}

void Worker::run()
{
	zmq::socket_t sock(context, ZMQ_REP);

	{
		usleep(10000);
		sock.connect(Aside::config->internal.c_str());
		sock.setsockopt(ZMQ_SNDBUF, &Aside::config->send_buffer_size, sizeof(Aside::config->send_buffer_size));
		sock.setsockopt(ZMQ_RCVBUF, &Aside::config->receive_buffer_size, sizeof(Aside::config->receive_buffer_size));
	}

	msgpack::sbuffer buffer_false;
	{
		typedef msgpack::packer<msgpack::sbuffer> Unpacker;
		Unpacker(buffer_false).pack_false();
	}
	zmq::message_t msg_false(buffer_false.data(), buffer_false.size(), NULL, NULL);

	while (true)
	{
		sock.recv(&recv_buf, 0);
		CS_SAY("received bytes: " << recv_buf.size());
		if (CS_BLIKELY(bus.route(recv_buf, send_buf)))
		{
			CS_SAY("processed, res bytes: " << send_buf.size());
			sock.send(send_buf);
		}
		else
		{
			CS_SAY("error occured, reply false");
			sock.send(msg_false);
		}
		CS_SAY("sent");
	}
}

} /* namespace rknn */
} /* namespace classify */
} /* namespace jebe */
