
#pragma once

#include "predef.hpp"
#include <memory>
#ifdef __linux
#	define BOOST_FILESYSTEM2_NARROW_ONLY
#endif
#include <mongo/client/dbclient.h>
#include "input.hpp"

namespace jebe {
namespace idf {

class MongoInput
	: public BaseInput
{
protected:
	std::auto_ptr<mongo::DBClientConnection> con;
	std::auto_ptr<mongo::DBClientCursor> cur;

private:
	std::string server;
	std::string collection;
	std::string field;

public:
	MongoInput();

	~MongoInput();

	void start();

	bool more();

	const char* next();

	void stop();
};

} /* namespace idf */
} /* namespace jebe */
