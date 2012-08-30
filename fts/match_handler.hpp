
#pragma once

#include "predef.hpp"
#include "handler.hpp"
#include <iostream>
#include <zmq.hpp>
#include <msgpack.hpp>
#include "index.hpp"

namespace jebe {
namespace fts {

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
	mutable std::vector<docid_t> docs;
	mutable DocWeightMap dwmap;

	msgpack::sbuffer packerBuffer;
	msgpack::packer<msgpack::sbuffer> packer;
	msgpack::unpacker unpacker;

protected:
	virtual HandleRes process(zmq::message_t& req, zmq::message_t& rep)
	{
		Index::getInstance()->match(getWordList(req), docs, dwmap);
		makeResponse(rep);
		return success;
	}

private:
	WordWeightList& getWordList(zmq::message_t& req) const
	{
		msgpack::unpacked msg;
		wws.clear();
		try
		{
			msgpack::unpack(&msg, reinterpret_cast<const char*>(req.data()) + 1, req.size() - 1);
			msg.get().convert(&wws);
		}
		catch (const msgpack::unpack_error& e)
		{
			// TODO log error.
			CS_ERR("error occured while unpacking request message: " << e.what());
			return wws;
		}
		CS_SAY("will convert to <WordWeightList>");

		for (WordWeightList::iterator it = wws.begin(); it != wws.end(); ++it)
		{
			CS_SAY("word: [" << it->word << "], weight: [" << it->weight << "]");
		}

		return wws;
	}

	void makeResponse(zmq::message_t& rep)
	{
		rep.rebuild();
		CS_DUMP(docs.size());
		if (docs.empty())
		{
			return;
		}
		else
		{
			packerBuffer.clear();
			packer.pack(docs);
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
