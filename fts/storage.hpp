
#pragma once

#include "predef.hpp"
#include <cassert>
#include <leveldb/db.h>
#include <msgpack.hpp>
#include <utility>
#include "config.hpp"

namespace jebe {
namespace fts {

class Storage {
public:
	Storage();

	virtual ~Storage();

	template<typename CallBack>
	void walk(CallBack& callback)
	{
		leveldb::Options options;
		options.block_size = 1 << 20;
		options.create_if_missing = false;
		leveldb::DB* db;
		leveldb::Status status = leveldb::DB::Open(options, Config::getInstance()->dbpath, &db);
		assert(status.ok());

		std::auto_ptr<leveldb::Iterator> it = db->NewIterator(leveldb::ReadOptions());
		std::auto_ptr<DocWeightList> list = new DocWeightList;
		for (it->SeekToFirst(); it->Valid(); it->Next())
		{
			list->clear();
			callback(it->key().ToString(), *list);
		}
	}

	void unpack(const leveldb::Slice& value, DocWeightList& list)
	{
		msgpack::unpacker unpacker(value.size());
		memcpy(unpacker.buffer(), value.data(), value.size());
		unpacker.buffer_consumed(value.size());

		msgpack::unpacked result;
		while (unpacker.next(&result))
		{
			const DocWeight& d = result.get().as<DocWeight>();
			CS_DUMP(d.docid);
			CS_DUMP(d.weight);
			list.push_back(d);
		}
	}
};

} /* namespace cws */
} /* namespace jebe */
