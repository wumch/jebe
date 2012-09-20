
#pragma once

#include "predef.hpp"
#include <iostream>
#include <zmq.hpp>
#include "filter.hpp"

namespace jebe {
namespace cws {

class Handler
{
public:
	enum HandleRes {success, failed};

protected:
	static const Filter* filter;

public:
	static void initialize(const std::string& pattenfile)
	{
		filter = new Filter(pattenfile);
	}

public:
	virtual ~Handler() = 0;

	bool handle(zmq::message_t& req, zmq::message_t& rep)
	{
		if (CS_BLIKELY(validate(req)))
		{
			if (CS_BLIKELY(handle_(req, rep) == success))
			{
				return true;
			}
		}
		return false;
	}

	virtual void reset()
	{

	}

protected:
	HandleRes handle_(zmq::message_t& req, zmq::message_t& rep)
	{
		return process(reinterpret_cast<byte_t*>(req.data()) + 1, req.size() - 1, rep);
	}

	virtual HandleRes process(const byte_t* content, tsize_t len, zmq::message_t& rep) = 0;

	virtual bool validate(zmq::message_t& req) const
	{
		return req.size() > 0;
	}
};

} /* namespace qdb */
} /* namespace jebe */
