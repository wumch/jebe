
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

	mutable std::string retrieve_buffer;
	mutable msgpack::unpacker unpacker;
	mutable msgpack::unpacked result;

public:
	// would better call this in bootstrap period.
	CS_FORCE_INLINE static const Storage* getInstance()
	{
		static Storage* instance = new Storage;
		return instance;
	}

	Storage()
	{
		build();
		std::size_t bufsize = Config::getInstance()->max_retrieve_elements * sizeof(WordWeight);
		unpacker.reserve_buffer(bufsize);
	}

	leveldb::DB* operator->()
	{
		return db;
	}

	virtual ~Storage()
	{
		delete db;
	}

	bool marve(const char* key, std::size_t keylen, WordWeightList& wws, std::size_t n) const
	{
		return _marve(leveldb::Slice(key, keylen), wws, n);
	}

protected:
	void build();

	void unpack(const leveldb::Slice& value, DocWeightList& list);

	bool _marve(const leveldb::Slice& key, WordWeightList& wws, std::size_t n) const
	{
		bool exists = retrieve(key);
		if (exists)
		{
			for (uint32_t i = 0; i < n && unpacker.next(&result); ++i)
			{
				wws.push_back(result.get().as<WordWeight>());
			}
		}
		return exists;
	}

	bool retrieve(const leveldb::Slice& key) const
	{
		unpacker.reset();
		bool ok = db->Get(leveldb::ReadOptions(), key, &retrieve_buffer).ok();
		if (ok)
		{
			memcpy(unpacker.buffer(), retrieve_buffer.data(), retrieve_buffer.size());
			unpacker.buffer_consumed(retrieve_buffer.size());
		}
		return ok;
	}

};

} /* namespace qdb */
} /* namespace jebe */
