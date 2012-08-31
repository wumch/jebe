
#include "storage.hpp"
#include <iostream>

namespace jebe {
namespace qdb {

void Storage::build()
{
	const Config* const config = Config::getInstance();
	options.create_if_missing = true;
	options.max_open_files = config->max_open_files;
	options.block_size = config->block_size;
	options.write_buffer_size = config->write_buffer_size;

	leveldb::Status status = leveldb::DB::Open(options, config->dbpath, &db);

	if (!status.ok())
	{
		CS_DIE(status.ToString());
	}
}

void Storage::unpack(const leveldb::Slice& value, DocWeightList& list)
{
	CS_DUMP(value.size());
	msgpack::unpacker unpacker;
	memcpy(unpacker.buffer(), value.data(), value.size());
	unpacker.buffer_consumed(value.size());
	msgpack::unpacked result;
	unpacker.next(&result);
	result.get().convert(&list);
}

} /* namespace qdb */
} /* namespace jebe */
