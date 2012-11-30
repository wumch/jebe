
#include "mongoinput.hpp"
#include <cassert>
#include "aside.hpp"
#include "config.hpp"
#include "input_document.hpp"

namespace jebe {
namespace cluster {
namespace ets {

MongoInput::MongoInput()
	: con(NULL),
	  server(Aside::config->mongo_server),
	  collection(Aside::config->mongo_collection),
	  field(Aside::config->mongo_field),
	  _id_field("_id"), url_field("url"), text_field("text"), title_field("title"),
	  empty_indoc(NULL)
{}

bool MongoInput::next(char* heap)
{
	mongo::BSONObj bson(nextBSON());
	if (CS_BLIKELY(!bson.isEmpty()))
	{
		mongo::BSONElement _id = bson.getField(_id_field);
		mongo::BSONElement url = bson.getField(url_field);
		mongo::BSONElement text = bson.getField(text_field);
		mongo::BSONElement title = bson.getField(title_field);
		if (CS_BLIKELY(_id.type() == mongo::String && url.type() == mongo::String && text.type() == mongo::String))
		{
			new (heap) InDocument(_id.valuestr(), _id.valuestrsize(), url.valuestr(), url.valuestrsize(), text.valuestr(), text.valuestrsize(), title.valuestr(), title.valuestrsize());
			return true;
		}
	}
	return false;
}

InDocument* MongoInput::nextDoc()
{
	mongo::BSONObj bson(nextBSON());
	if (CS_BLIKELY(!bson.isEmpty()))
	{
		mongo::BSONElement _id = bson.getField(_id_field);
		mongo::BSONElement url = bson.getField(url_field);
		mongo::BSONElement text = bson.getField(text_field);
		mongo::BSONElement title = bson.getField(title_field);
		if (CS_BLIKELY(_id.type() == mongo::String && url.type() == mongo::String && text.type() == mongo::String))
		{
			return new InDocument(_id.valuestr(), _id.valuestrsize(), url.valuestr(), url.valuestrsize(), text.valuestr(), text.valuestrsize(), title.valuestr(), title.valuestrsize());
		}
	}
	return empty_indoc;
}

mongo::BSONObj MongoInput::nextBSON()
{
	static size_t got_docs = 0;
	CS_RETURN_IF(Aside::config->mongo_max_doc > 0 && got_docs > Aside::config->mongo_max_doc, empty_bson);

	if (CS_BLIKELY(more()))
	{
		__sync_add_and_fetch(&got_docs, 1);
		return cur->nextSafe();
	}
	else
	{
		return empty_bson;
	}
}

InDocument* MongoInput::next()
{
	return nextDoc();
}

void MongoInput::start()
{
	con.reset(new mongo::DBClientConnection(Aside::config->mongo_auto_reconnect));
	std::string error;
	if (!con->connect(server, error))
	{
		LOG_IF(ERROR, Aside::config->loglevel > 0) << "connect to mongodb-server [" << server << "] faield: " << error;
	}
	mongo::BSONObj fields(BSON("_id" << 1 << "text" << 1 << "url" << 1 << "title" << 1));
	cur = con->query(collection, mongo::Query().sort("$natural", 1), Aside::config->mongo_max_doc, 0, &fields);
}

bool MongoInput::more() const
{
	return cur->more();
}

void MongoInput::stop()
{
	if (cur.get() != NULL)
	{
		delete cur.release();
	}
	delete con.release();		// just close the connection, and release the db-client-cursor.
}

MongoInput::~MongoInput()
{
	stop();
}

} /* namespace ets */
} /* namespace cluster */
} /* namespace jebe */
