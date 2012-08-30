
#pragma once

#include "predef.hpp"
#include "handler.hpp"
#include <zmq.hpp>
#include <msgpack.hpp>
#include "index.hpp"

namespace jebe {
namespace fts {

class MatchHandler
	: public Handler
{
private:
	// assist temporary holders.
	mutable WordWeightList wws;
	mutable std::vector<docid_t> docs;

protected:
	virtual HandleRes process(zmq::message_t& req, zmq::message_t& rep) const
	{
		Index::getInstance()->match(getWordList(req), docs);
		makeResponse(rep);
		return success;
	}

private:
	WordWeightList& getWordList(zmq::message_t& req) const
	{
		msgpack::unpacked msg;
		try
		{
			msgpack::unpack(&msg, reinterpret_cast<const char*>(req.data()) + 1, req.size() - 1);
		}
		catch (const msgpack::unpack_error& e)
		{
			// TODO log error.
			CS_ERR("error occured while unpacking request message: " << e.what());
			return wws;
		}
		msgpack::object obj =
		msg.get();//.as<WordWeightList>();
		CS_SAY("obj.type: [" << obj.type << "]");
		wws.clear();
		CS_SAY("will convert to <WordWeightList>");
		obj.convert(&wws);

		for (WordWeightList::iterator it = wws.begin(); it != wws.end(); ++it)
		{
			CS_SAY("word: [" << it->word << "], weight: [" << it->weight << "]");
		}

		return wws;
	}

	void makeResponse(zmq::message_t& rep) const
	{

	}

	virtual ~MatchHandler()
	{

	}

};

}
}
