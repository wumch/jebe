
#pragma once

#include "predef.hpp"
#include <vector>
#include <limits.h>
#include <boost/noncopyable.hpp>
#include <zmq.hpp>
#include "config.hpp"
#include "handler.hpp"
#include "classify_handler.hpp"

namespace jebe {
namespace classify {
namespace rknn {

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
		BOOST_STATIC_ASSERT(sizeof(uint8_t) == 1);
		return *reinterpret_cast<const uint8_t*>(req.data());
	}

	void initHandlers()
	{
		Handler::initialize();
		handlers[classify] = new ClassifyHandler;
	}

	void handleError(zmq::message_t& req, zmq::message_t& rep) const
	{
		// nothing to do... maybe log sth...
	}
};

} /* namespace rknn */
} /* namespace classify */
} /* namespace jebe */
