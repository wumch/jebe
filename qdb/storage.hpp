
#pragma once

#include "predef.hpp"
#include <string>
#include <utility>
#include <boost/pool/pool_alloc.hpp>
#include <boost/bind.hpp>
#include <leveldb/db.h>
#include "hash.hpp"
#include "config.hpp"

namespace jebe {
namespace qdb {

class Storage
{
private:
	leveldb::Options options;
	leveldb::DB* db;

public:
	// would better call this in bootstrap period.
	CS_FORCE_INLINE static Storage* getInstance()
	{
		static Storage* instance = new Storage;
		return instance;
	}

	Storage()
	{
		build();
	}

	leveldb::DB* operator->()
	{
		return db;
	}

	virtual ~Storage()
	{
		db->~DB();
		delete db;
	}

	bool marve(const std::string& key, WordWeightList& wws, std::size_t n)
	{
		return _marve(leveldb::Slice(key), wws, n);
	}

	bool store(const std::string& key, const std::string& value)
	{
		return _store(leveldb::Slice(key), leveldb::Slice(value));
	}

protected:
	void build();

	void unpack(const leveldb::Slice& value, DocWeightList& list);

	bool _store(const leveldb::Slice& key, const leveldb::Slice& value)
	{
		return db->Put(leveldb::WriteOptions(), key, value).ok();
	}

	bool _marve(const leveldb::Slice& key, WordWeightList& wws, std::size_t n)
	{
		CS_SAY(__LINE__);
		std::string retrieve_buffer;
		bool exists = retrieve(key, retrieve_buffer);
		CS_SAY(__LINE__);

		if (exists)
		{
			msgpack::unpacked res;
			msgpack::unpack(&res, retrieve_buffer.data(), retrieve_buffer.size());
			res.get().convert(&wws);
		}
		return exists;
	}

	bool retrieve(const leveldb::Slice& key, std::string& retrieve_buffer)
	{
		CS_SAY(__LINE__);
		bool ok = false;
		try
		{
			ok = db->Get(leveldb::ReadOptions(), key, &retrieve_buffer).ok();
		}
		catch (const std::exception& e)
		{
			CS_SAY("error occured while <leveldb::DB>.Get: " << e.what());
			return false;
		}
		CS_SAY("ok:" << ok);
		if (ok)
		{
			msgpack::unpacker unpacker;
			CS_SAY(__LINE__);
			memcpy(unpacker.buffer(), retrieve_buffer.data(), retrieve_buffer.size());
			CS_SAY(__LINE__);
			unpacker.buffer_consumed(retrieve_buffer.size());
			CS_SAY(__LINE__);
		}
		return ok;
	}

};

} /* namespace qdb */
} /* namespace jebe */
