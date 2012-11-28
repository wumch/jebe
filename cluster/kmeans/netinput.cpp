
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
#include "../rconfig.hpp"

namespace jebe {
namespace cluster {
namespace kmeans {

NetInput::NetInput()
	: context(Config::getInstance()->io_threads),
	  sock(context, ZMQ_REP),
	  recv_buf_area(new char[Aside::config->receive_buffer_size]),
	  recv_buf(recv_buf_area, Aside::config->receive_buffer_size, NULL, NULL),
	  send_buf_area(new char[Aside::config->send_buffer_size]),
	  send_buf(send_buf_area, Config::getInstance()->send_buffer_size, NULL, NULL)
{}

Document* NetInput::next()
{
	recv_buf.rebuild(recv_buf_area, Aside::config->receive_buffer_size, NULL, NULL);
	sock.recv(&recv_buf, 0);
	return handleAction(getAction());
}

Document* NetInput::handleAction(Action act)
{
	if (CS_BLIKELY(act == send_doc))
	{
		handleDoc();
		sock.send(send_buf);
		return cur;
	}
	else if (CS_BUNLIKELY(act == tell_total))
	{
		handleTotal();
		sock.send(send_buf);
		return next();
	}
	else if (CS_BUNLIKELY(act == tell_config))
	{
		handleConfig();
		sock.send(send_buf);
		return next();
	}
	else if (CS_BUNLIKELY(act == thats_all))
	{
		LOG_IF(INFO, Aside::config->loglevel > 0) << "finished inputing, received " << Aside::curVecNum << " vectors";
		handleThatSAll();
		return NULL;
	}
	else
	{
		send_buf.copy(&failed_response);
		sock.send(send_buf);
		return next();
	}
}

Action NetInput::getAction()
{
	return (CS_BLIKELY(recv_buf.size() > 0)) ?
		static_cast<Action>(*reinterpret_cast<unsigned char*>(recv_buf.data())) : wrong;
}

void NetInput::handleTotal()
{
	msgpack::unpacked msg;
	msgpack::unpack(&msg, reinterpret_cast<char*>(recv_buf.data()) + 1, recv_buf.size() - 1);
	Aside::totalVecNum += msg.get().as<vnum_t>();
	send_buf.copy(&success_response);
	CS_SAY("client said total count of documents is " << Aside::totalVecNum);
}

void NetInput::handleDoc()
{
	resetCurDoc(reinterpret_cast<char*>(recv_buf.data()) + 1, recv_buf.size() - 1);
	send_buf.copy(&success_response);
}

void NetInput::handleThatSAll()
{
	LOG_IF(INFO, Aside::config->loglevel > 0) << "finished inputing, received " << Aside::curVecNum << " vectors";
	send_buf.copy(&success_response);
	sock.send(send_buf);
	stop();
	Aside::caler->calcu();
}

void NetInput::handleConfig()
{
	size_t moffset = 0;
	msgpack::zone mzone;
	msgpack::object mobj;
	if (CS_BLIKELY(msgpack::unpack(reinterpret_cast<char*>(recv_buf.data()) + 1, recv_buf.size() - 1, &moffset, &mzone, &mobj) == msgpack::UNPACK_SUCCESS))
	{
		mobj.convert(Aside::rconfig);
		Aside::overrideConfig();
		Aside::rconfig->output();
		send_buf.copy(&success_response);
	}
	else
	{
		send_buf.copy(&failed_response);
		CS_DIE("kid, error occured while processing remote-config");
	}
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

NetInput::~NetInput()
{
	delete cur;
}

} /* namespace kmeans */
} /* namespace cluster */
} /* namespace jebe */
