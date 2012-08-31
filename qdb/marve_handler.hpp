
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

class MatchHandler
	: public Handler
{
public:
	MatchHandler()
		: //packerBuffer(Config::getInstance()->receive_buffer_size),
		  packer(&packerBuffer)
	{
		unpacker.reserve_buffer(Config::getInstance()->receive_buffer_size);
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
		bool exists = Storage::getInstance()->marve(reinterpret_cast<char*>(req.data()), req.size(), wws, 20);
		makeResponse(rep, exists);
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

	virtual ~MatchHandler()
	{

	}

};

}
}
