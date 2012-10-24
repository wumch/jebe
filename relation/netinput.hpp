
#pragma once

#include "predef.hpp"
#include <zmq.hpp>
#include "input.hpp"

namespace jebe {
namespace rel {

class NetInput
	: public BaseInput
{
	enum Action {
		wrong = 0,
		tellTotal = 1,
		sendDoc	= 2,
	};

protected:
	virtual void handleInput();

	void handleInput(Action act);

	Document* handleAction(Action act);

	void handleDoc();

	void handleTotal();

	Action getAction();

public:
	NetInput();

	virtual void prepare();

	virtual void start();

	virtual void stop();

	virtual Document* next();

	virtual ~NetInput();

private:
	zmq::context_t context;
	zmq::socket_t sock;

	zmq::message_t recv_buf;
	zmq::message_t send_buf;

	zmq::message_t success_response;
	zmq::message_t failed_response;
};

} /* namespace fts */
} /* namespace jebe */
