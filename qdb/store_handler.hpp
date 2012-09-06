
#pragma once

#include "predef.hpp"
#include "handler.hpp"
#include <iostream>
#include <zmq.hpp>
#include <msgpack.hpp>
#include "config.hpp"
#include "storage.hpp"

namespace jebe {
namespace qdb {

class StoreHandler
	: public Handler
{
private:
	msgpack::sbuffer msg_true;
	msgpack::sbuffer msg_false;

public:
	StoreHandler()
	{
		typedef msgpack::packer<msgpack::sbuffer> Unpacker;
		Unpacker(msg_true).pack_true();
		Unpacker(msg_false).pack_false();
	}

private:
	// assist temporary holders.

protected:
	virtual HandleRes process(zmq::message_t& req, zmq::message_t& rep)
	{
		int reqsize = req.size() - 1;
		msgpack::unpacker unpacker(reqsize + 16);
		unpacker.reserve_buffer(reqsize);
		memcpy(unpacker.buffer(), reinterpret_cast<char*>(req.data()) + 1, reqsize);
		try
		{
			unpacker.buffer_consumed(reqsize);
			msgpack::unpacked result;
			unpacker.next(&result);
			std::string key(result.get().as<std::string>());
			CS_SAY("key: [" << key << "]");
			unpacker.next(&result);
			std::string value(result.get().as<std::string>());
			bool succed = Storage::getInstance()->store(key, value);
			makeResponse(rep, succed);
			return success;
		}
		catch (const std::exception& e)
		{
			CS_ERR("error occured while msgpack.unpacking: " << e.what());
		}
		return failed;
	}

private:
	void makeResponse(zmq::message_t& rep, bool succed)
	{
		if (succed)
		{
			rep.rebuild(msg_true.data(), msg_true.size(), NULL, NULL);
		}
		else
		{
			rep.rebuild(msg_false.data(), msg_false.size(), NULL, NULL);
		}
	}

	virtual ~StoreHandler()
	{

	}

};

}
}
