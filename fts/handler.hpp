
#pragma once

#include "predef.hpp"
#include <zmq.hpp>

namespace jebe {
namespace fts {

class Handler
{
public:
	enum HandleRes {success, failed};

public:
	virtual ~Handler() = 0;

	void handle(zmq::message_t& req, zmq::message_t& rep)
	{
		if (CS_BLIKELY(validate(req)))
		{
			if (CS_BUNLIKELY(process(req, rep) != success))
			{
				// nothing to, maybe log sth.
			}
		}
	}

protected:
	virtual HandleRes process(zmq::message_t& req, zmq::message_t& rep) const = 0;

	virtual bool validate(zmq::message_t& req) const
	{
		return true;
	}
};

} /* namespace fts */
} /* namespace jebe */
