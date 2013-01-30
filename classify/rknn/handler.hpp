
#pragma once

#include "predef.hpp"
#include <iostream>
#include <zmq.hpp>
#include "knn.hpp"

namespace jebe {
namespace classify {
namespace rknn {

class Handler
{
public:
	enum HandleRes {success, failed};

protected:
	static Knn* knn;

public:
	static void initialize()
	{
		knn = new Knn(Aside::config->k);
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

protected:
	HandleRes handle_(zmq::message_t& req, zmq::message_t& rep)
	{
		return process(reinterpret_cast<char*>(req.data()) + 1, req.size() - 1, rep);
	}

	virtual HandleRes process(const char* content, size_t len, zmq::message_t& rep) = 0;

	virtual bool validate(zmq::message_t& req) const
	{
		return req.size() > 0;
	}
};

} /* namespace rknn */
} /* namespace classify */
} /* namespace jebe */
