
#pragma once

#include "predef.hpp"
#include <iostream>
#include <zmq.hpp>
#include "math.hpp"
#include "seger.hpp"

namespace jebe {
namespace ics {

class Handler
{
public:
	enum HandleRes {success, failed};

protected:
	static Seger* seger;

public:
	static void initialize(const std::string& pattenfile)
	{
		seger = new Seger;
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
		return process(reinterpret_cast<char*>(req.data()) + 1, req.size() - 1, rep);
	}

	virtual HandleRes process(char* content, tsize_t len, zmq::message_t& rep) = 0;

	virtual bool validate(zmq::message_t& req) const
	{
		return staging::between_open(req.size(), 0ul, Config::getInstance()->msg_max_size);
	}
};

} /* namespace qdb */
} /* namespace jebe */
