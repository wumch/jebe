
#pragma once

#include "predef.hpp"
#include <zmq.hpp>
#include "input.hpp"
#include "aside.hpp"
#include "../document.hpp"

namespace jebe {
namespace cluster {
namespace kmeans {

class NetInput
	: public BaseInput
{
//	enum Action {
//		wrong = 0,
//		tellTotal = 1,
//		sendDoc	= 2,
//		thatSAll = 99,
//	};

protected:
	Document* handleAction(Action act);

	void handleDoc();

	void handleTotal();

	void handleThatSAll();

	void handleConfig();

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

	char* recv_buf_area;
	zmq::message_t recv_buf;
	char* send_buf_area;
	zmq::message_t send_buf;

	zmq::message_t success_response;
	zmq::message_t failed_response;
};

} /* namespace kmeans */
} /* namespace cluster */
} /* namespace jebe */
