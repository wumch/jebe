
#include "netinput.hpp"
#ifdef __linux
#	include <unistd.h>
#	include <sys/prctl.h>
#endif
#include <zmq.hpp>
#include <msgpack.hpp>
#include "config.hpp"

namespace jebe {
namespace rel {

NetInput::NetInput()
	: context(Config::getInstance()->io_threads),
	  sock(context, ZMQ_REP),
	  recv_buf(Config::getInstance()->receive_buffer_size),
	  send_buf(Config::getInstance()->send_buffer_size)
{
}

void NetInput::prepare()
{
	msgpack::sbuffer packerBuffer;
	msgpack::packer<msgpack::sbuffer> packer(&packerBuffer);

	{
		packerBuffer.clear();
		packer.pack_true();
		char* bytes = new char[packerBuffer.size()];
		memcpy(bytes, packerBuffer.data(), packerBuffer.size());
		success_response.rebuild(bytes, packerBuffer.size(), NULL, NULL);
	}

	{
		packerBuffer.clear();
		packer.pack_false();
		char* bytes = new char[packerBuffer.size()];
		memcpy(bytes, packerBuffer.data(), packerBuffer.size());
		failed_response.rebuild(bytes, packerBuffer.size(), NULL, NULL);
	}
}

void NetInput::start()
{
	const Config* const config = Config::getInstance();
#ifdef __linux
	prctl(PR_SET_NAME, (config->program_name + ":worker").c_str());
#endif
	sock.bind(config->listen.c_str());
	sock.setsockopt(ZMQ_SNDBUF, &config->send_buffer_size, sizeof(config->send_buffer_size));
	sock.setsockopt(ZMQ_RCVBUF, &config->receive_buffer_size, sizeof(config->receive_buffer_size));
}

void NetInput::stop()
{
	sock.close();
}

Document* NetInput::next()
{
	CS_SAY("waiting for incoming");
	recv_buf.rebuild();
	sock.recv(&recv_buf, 0);
	CS_SAY("received bytes: " << recv_buf.size());
	handleInput();
	sock.send(send_buf);
	CS_SAY("processed, res bytes: " << success_response.size());
	return cur;
}

void NetInput::handleTotal()
{
	msgpack::unpacked msg;
	msgpack::unpack(&msg, reinterpret_cast<char*>(recv_buf.data()) + 1, recv_buf.size() - 1);
	total_ = msg.get().as<docnum_t>();
}

void NetInput::handleInput()
{
	handleInput(getAction());
}

void NetInput::handleInput(Action act)
{
	if (CS_BLIKELY(act == sendDoc))
	{
		resetCurDoc(reinterpret_cast<char*>(recv_buf.data()) + 1, recv_buf.size() - 1);
		send_buf.copy(&success_response);
	}
	else if (CS_BUNLIKELY(act == tellTotal))
	{
		handleTotal();
		send_buf.copy(&success_response);
	}
	else
	{
		send_buf.copy(&failed_response);
	}
}

docnum_t NetInput::total() const
{
	return total_;
}

NetInput::Action NetInput::getAction()
{
	return (CS_BLIKELY(recv_buf.size() > 1)) ?
		static_cast<Action>(*reinterpret_cast<unsigned char*>(recv_buf.data())) : wrong;
}

NetInput::~NetInput()
{
	delete cur;
}

} /* namespace fts */
} /* namespace jebe */
