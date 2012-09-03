
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
		wws.reserve(50);
		storer = Storage::getInstance();
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

	std::string curkey;

	Storage* storer;

protected:
	virtual HandleRes process(zmq::message_t& req, zmq::message_t& rep)
	{
		{
			msgpack::unpacked result;
			// I'd rather die if client do harm to me.
			int reqsize = req.size() - 1;
			msgpack::unpacker unpacker(reqsize + 16);	// 16 bytes for filling the fucking "4 bytes gap".
			unpacker.reserve_buffer(reqsize);
			memcpy(unpacker.buffer(), reinterpret_cast<char*>(req.data()) + 1, reqsize);
			unpacker.buffer_consumed(reqsize);
			try
			{
				unpacker.next(&result);
				curkey = result.get().as<std::string>();
			}
			catch (const std::exception& e)
			{
				CS_ERR("error occured while unpacking `key` inside <MarveHandler>.process: "
						<< e.what() << ", request-message.size(): " << req.size());
				return failed;
			}
		}

		wws.clear();
		bool exists = storer->marve(curkey, wws, 20);
		makeResponse(rep, exists);
		return success;
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
