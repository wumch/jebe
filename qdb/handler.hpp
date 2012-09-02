
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

	bool handle(zmq::message_t& req, zmq::message_t& rep)
	{
		if (CS_BLIKELY(validate(req)))
		{
			if (CS_BLIKELY(process(req, rep) == success))
			{
				return true;
			}
		}
		return false;
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
