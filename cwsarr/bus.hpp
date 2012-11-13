
#pragma once

#include "predef.hpp"
#include <vector>
#include <limits.h>
#include <boost/noncopyable.hpp>
#include <zmq.hpp>
#include "config.hpp"
#include "filter.hpp"
#include "handler.hpp"
#include "marve_handler.hpp"
//#include "split_handler.hpp"

namespace jebe {
namespace cws {

enum Action {
	// reserve 0-9 for system.
	marve = 11,
	split = 12,
	count = 13,
	compare = 14,
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
		Handler* handler = handlers[getAction(req)];
		if (CS_BLIKELY(handler != NULL))
		{
			CS_SAY("action exists");
			handler->reset();
			return handler->handle(req, rep);
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
		Handler::initialize(Config::getInstance()->getInstance()->pattenfile.string());
		handlers[marve] = new MarveHandler;
//		handlers[split] = new SplitHandler;
//		handlers[count];
//		handlers[compare];
	}

	void handleError(zmq::message_t& req, zmq::message_t& rep) const
	{
		// nothing to do... maybe log sth...
	}
};

} /* namespace qdb */
} /* namespace jebe */
