
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

class MarveHandler
	: public Handler
{
public:
	MarveHandler()
	{
	}

private:
	// assist temporary holders.
	mutable WordWeightList wws;

protected:
	virtual HandleRes process(zmq::message_t& req, zmq::message_t& rep)
	{
		msgpack::unpacker unpacker;
		memcpy(unpacker.buffer(), reinterpret_cast<char*>(req.data()) + 1, req.size() - 1);
		unpacker.buffer_consumed(req.size() - 1);
		msgpack::unpacked result;
		unpacker.next(&result);

		try
		{
			std::string key(result.get().as<std::string>());
			bool exists = Storage::getInstance()->marve(key, wws, 20);
			makeResponse(rep, exists);
			return success;
		}
		catch (const std::exception& e)
		{
			CS_ERR("error occured while unpacking `key` inside <MarveHandler>.process: " << e.what() << ", request-message.size(): " << req.size());
		}
		return failed;
	}

private:
	void makeResponse(zmq::message_t& rep, bool exists)
	{
		msgpack::sbuffer packerBuffer;
		msgpack::packer<msgpack::sbuffer> packer(packerBuffer);
		rep.rebuild();
		if (!exists)
		{
			packerBuffer.clear();
			packer.pack_false();
		}
		else if (wws.empty())
		{
			return;
		}
		else
		{
			packerBuffer.clear();
			packer.pack(wws);
		}

		if (packerBuffer.size())
		{
			rep.rebuild(packerBuffer.data(), packerBuffer.size(), NULL, NULL);
		}
	}

	virtual ~MarveHandler()
	{

	}

};

}
}
