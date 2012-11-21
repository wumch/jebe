
#pragma once

#include "predef.hpp"
#include <memory>
#ifdef __linux
#	define BOOST_FILESYSTEM2_NARROW_ONLY
#endif
#include <mongo/client/dbclient.h>
#include "input.hpp"
#include "document.hpp"

namespace jebe {
namespace cluster {
namespace preprocess {

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

	InDocument empty_indoc;
	mongo::BSONObj empty_bson;

public:
	MongoInput();

	~MongoInput();

	void start();

	bool more();

	InDocument next();

	bool next(char* heap);

	void stop();

private:
	InDocument nextDoc();

	mongo::BSONObj nextBSON();
};

} /* namespace preprocess */
} /* namespace cluster */
} /* namespace jebe */
