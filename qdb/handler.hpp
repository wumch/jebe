
#pragma once

#include "predef.hpp"
#include <iostream>
#include <zmq.hpp>

namespace jebe {
namespace qdb {

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
				CS_ERR("handle request failed");
			}
		}
	}

protected:
	virtual HandleRes process(zmq::message_t& req, zmq::message_t& rep) = 0;

	virtual bool validate(zmq::message_t& req) const
	{
		return req.size() > 0;
	}
};

} /* namespace qdb */
} /* namespace jebe */
