
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
		: packer(packerBuffer)
	{
	}

private:
	// assist temporary holders.
	mutable WordWeightList wws;

	msgpack::sbuffer packerBuffer;
	msgpack::packer<msgpack::sbuffer> packer;
	msgpack::unpacker unpacker;

protected:
	virtual HandleRes process(zmq::message_t& req, zmq::message_t& rep)
	{
		unpacker.reset();
		CS_SAY(__LINE__);
		memcpy(unpacker.buffer(), reinterpret_cast<char*>(req.data()) + 1, req.size() - 1);
		CS_SAY(__LINE__);
		unpacker.buffer_consumed(req.size() - 1);
		CS_SAY(__LINE__);
		msgpack::unpacked result;
		CS_SAY(__LINE__);
		unpacker.next(&result);
		CS_SAY(__LINE__);
		std::string key(result.get().as<std::string>());
		CS_SAY(__LINE__);
		bool exists = Storage::getInstance()->marve(key, wws, 20);
		CS_SAY(__LINE__);
		makeResponse(rep, exists);
		CS_SAY(__LINE__);
		return success;
	}

private:
	void makeResponse(zmq::message_t& rep, bool exists)
	{
		rep.rebuild();
		CS_DUMP(wws.size());
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

		CS_DUMP(packerBuffer.size());

		if (packerBuffer.size())
		{
			rep.rebuild(packerBuffer.data(), packerBuffer.size(), NULL, NULL);
		}
		CS_DUMP(rep.size());
	}

	virtual ~MarveHandler()
	{

	}

};

}
}
