
#pragma once

#include "predef.hpp"
#include <vector>
#include <limits.h>
#include <boost/ptr_container/ptr_array.hpp>
#include <boost/noncopyable.hpp>
#include <zmq.hpp>
#include "handler.hpp"
#include "match_handler.hpp"

namespace jebe {
namespace fts {

enum Action {
	// reserve 0-9 for system.
	match = 10,
};

// route received message.
class Bus
	: private boost::noncopyable
{
private:
//	typedef boost::ptr_array<Handler, UCHAR_MAX + 1> HandlerList;
	typedef std::vector<Handler*> HandlerList;

	HandlerList handlers;

public:
	Bus()
		: handlers(UCHAR_MAX + 1, NULL)
	{
		initHandlers();
	}

	void route(zmq::message_t& req, zmq::message_t& rep)
	{
		if (CS_BLIKELY(handlers[getAction(req)] != NULL))
		{
			handlers[getAction(req)]->handle(req, rep);
		}
		else
		{
			handleError(req, rep);
		}
	}

protected:
	uint8_t getAction(zmq::message_t& req) const
	{
		return *reinterpret_cast<const uint8_t*>(req.data());
	}

	void initHandlers()
	{
		handlers[match] = new MatchHandler;
	}

	void handleError(zmq::message_t& req, zmq::message_t& rep) const
	{
		// should also reply sth.
	}
};

} /* namespace fts */
} /* namespace jebe */
