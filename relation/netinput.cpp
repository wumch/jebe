
#include "netinput.hpp"
#ifdef __linux
#	include <unistd.h>
#	include <sys/prctl.h>
#endif
#include <zmq.hpp>
#include <msgpack.hpp>
#include "config.hpp"
#include "aside.hpp"
#include "calculater.hpp"

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
	Aside::totalDocNum = Aside::curDocNum;
}

Document* NetInput::next()
{
	recv_buf.rebuild();
	sock.recv(&recv_buf, 0);
	return handleAction(getAction());
}

void NetInput::handleTotal()
{
	msgpack::unpacked msg;
	msgpack::unpack(&msg, reinterpret_cast<char*>(recv_buf.data()) + 1, recv_buf.size() - 1);
	Aside::totalDocNum += msg.get().as<docnum_t>();
	send_buf.copy(&success_response);
	CS_SAY("client said total count of documents is " << Aside::totalDocNum);
}

void NetInput::handleDoc()
{
	resetCurDoc(reinterpret_cast<char*>(recv_buf.data()) + 1, recv_buf.size() - 1);
	send_buf.copy(&success_response);
}

void NetInput::handleThatSAll()
{
	send_buf.copy(&success_response);
	Aside::caler->calculate();
}

Document* NetInput::handleAction(Action act)
{
	if (CS_BLIKELY(act == sendDoc))
	{
		handleDoc();
		sock.send(send_buf);
		return cur;
	}
	else if (CS_BUNLIKELY(act == tellTotal))
	{
		handleTotal();
		sock.send(send_buf);
		return next();
	}
	else if (CS_BUNLIKELY(act == thatSAll))
	{
		handleThatSAll();
		sock.send(send_buf);
		return NULL;
	}
	else
	{
		send_buf.copy(&failed_response);
		sock.send(send_buf);
		return next();
	}
}

NetInput::Action NetInput::getAction()
{
	return (CS_BLIKELY(recv_buf.size() > 0)) ?
		static_cast<Action>(*reinterpret_cast<unsigned char*>(recv_buf.data())) : wrong;
}

NetInput::~NetInput()
{
	delete cur;
}

} /* namespace fts */
} /* namespace jebe */
