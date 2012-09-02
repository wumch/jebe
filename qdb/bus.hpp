
#pragma once

#include "predef.hpp"
#include <vector>
#include <limits.h>
#include <boost/noncopyable.hpp>
#include <zmq.hpp>
#include "handler.hpp"
#include "marve_handler.hpp"
#include "store_handler.hpp"

namespace jebe {
namespace qdb {

enum Action {
	// reserve 0-9 for system.
	marve = 11,
	store = 101,
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

	bool route(zmq::message_t& req, zmq::message_t& rep)
	{
		CS_SAY("action: [" << static_cast<int>(getAction(req)) << "]");
		if (CS_BLIKELY(handlers[getAction(req)] != NULL))
		{
			CS_SAY("action exists");
			return handlers[getAction(req)]->handle(req, rep);
		}
		else
		{
			CS_SAY("action non-exists");
			handleError(req, rep);
			return false;
		}
	}

protected:
	uint8_t getAction(zmq::message_t& req) const
	{
		return *reinterpret_cast<const uint8_t*>(req.data());
	}

	void initHandlers()
	{
		handlers[marve] = new MarveHandler;
		handlers[store] = new StoreHandler;
	}

	void handleError(zmq::message_t& req, zmq::message_t& rep) const
	{
		// should also reply sth.
	}
};

} /* namespace qdb */
} /* namespace jebe */
