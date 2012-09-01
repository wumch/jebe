
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
		: falsePacker(falsePackerBuffer),
		  nullPacker(nullPackerBuffer),
		  packer(packerBuffer)
	{
		falsePacker.pack_false();
		nullPacker.pack_nil();
	}

private:
	// assist temporary holders.
	WordWeightList wws;

	msgpack::sbuffer falsePackerBuffer;
	msgpack::packer<msgpack::sbuffer> falsePacker;

	msgpack::sbuffer nullPackerBuffer;
	msgpack::packer<msgpack::sbuffer> nullPacker;

	msgpack::sbuffer packerBuffer;
	msgpack::packer<msgpack::sbuffer> packer;

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
			wws.clear();		// SGI-STL only
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
		if (!exists)
		{
			rep.rebuild(falsePackerBuffer.data(), falsePackerBuffer.size(), NULL, NULL);
		}
		else if (wws.empty())
		{
			rep.rebuild(nullPackerBuffer.data(), nullPackerBuffer.size(), NULL, NULL);
		}
		else
		{
			packerBuffer.clear();
			packer.pack(wws);
			rep.rebuild(packerBuffer.data(), packerBuffer.size(), NULL, NULL);
		}
	}

	virtual ~MarveHandler()
	{

	}

};

}
}
