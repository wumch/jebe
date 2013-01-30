
#pragma once

#include "predef.hpp"
#include <boost/unordered_map.hpp>
#include <msgpack.hpp>
#include "handler.hpp"

namespace jebe {
namespace classify {
namespace rknn {

class ClassifyHandler
	: public Handler
{
private:
	ClassifyResult cres;
	char* resbuf;

public:
	ClassifyHandler()
		: resbuf(new char[sizeof(ClassifyResult)])
	{}

	virtual HandleRes process(const char* content, size_t len, zmq::message_t& rep)
	{
		Vector vec(InputVector(content, len));
		// TODO: encapsulation by <Calculator>.
//		cres.cls = knn->classify(vec);

		zmq::message_t message(reinterpret_cast<char*>(&cres), sizeof(cres), NULL, NULL);
		rep.copy(&message);

		return success;
	}

	virtual ~ClassifyHandler()
	{
		delete resbuf;
	}
};

}
}
}
