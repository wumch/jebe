
#include "mongoinput.hpp"
#include <glog/logging.h>
#include <mongo/client/dbclient.h>
#include <cassert>
#include "aside.hpp"
#include "config.hpp"

namespace jebe {
namespace idf {

MongoInput::MongoInput()
	: con(NULL),
	  server(Aside::config->mongo_server),
	  collection(Aside::config->mongo_collection),
	  field(Aside::config->mongo_field)
{}

const char* MongoInput::next()
{
	return CS_BLIKELY(more()) ? cur->nextSafe().getStringField(field.c_str()) : NULL;
}

void MongoInput::start()
{
	con.reset(new mongo::DBClientConnection(Aside::config->mongo_auto_reconnect));
	std::string error;
	if (!con->connect(server, error))
	{
		LOG_IF(ERROR, Aside::config->loglevel > 0) << "connect to mongodb-server [" << server << "] faield: " << error;
	}
	cur = con->query(collection, mongo::Query().sort("$natural", -1));
}

bool MongoInput::more()
{
	return cur->more();
}

void MongoInput::stop()
{
	if (cur.get() != NULL)
	{
		delete cur.release();
	}
	delete con.release();		// just close, and release the db-client-cursor
}

MongoInput::~MongoInput()
{
	stop();
}

} /* namespace idf */
} /* namespace jebe */
