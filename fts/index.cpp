
#include "index.hpp"
#include <iostream>

namespace jebe {
namespace fts {

Index::Index()
{
	build();
}

void Index::match(const WordWeightList& words, std::vector<docid_t>& docids, DocWeightMap dwmap, std::size_t max_match) const
{
	dwmap.clear();
	docids.clear();

	for (WordWeightList::const_iterator wit = words.begin(); wit != words.end(); ++wit)
	{
		MapType::const_iterator entry = map.find(wit->word);
		if (entry != map.end())
		{
			const DocWeightList& docs = entry->second;
			for (DocWeightList::const_iterator dit = docs.begin(); dit != docs.end(); ++dit)
			{
				DocWeightMap::iterator doc = dwmap.find(dit->docid);
				if (doc == dwmap.end())
				{
					dwmap[dit->docid] = dit->weight * wit->weight;
				}
				else
				{
					doc->second += dit->weight * wit->weight;
				}
			}

			if (CS_BUNLIKELY(!(dwmap.size() < Config::getInstance()->max_match)))
			{
				break;
			}
		}
	}

	// TODO: currently find the most high one.
	marve_t min_marve = .0;
	for (DocWeightMap::const_iterator it = dwmap.begin(); it != dwmap.end(); ++it)
	{
		if (it->second >= min_marve)
		{
			if (docids.empty())
			{
				docids.push_back(it->first);
			}
			else
			{
				docids[0] = it->first;
			}
			min_marve = it->second;
		}
	}
}

void Index::build()
{
	leveldb::Options options;
	options.create_if_missing = false;
	leveldb::DB* db;
	leveldb::Status status = leveldb::DB::Open(options, Config::getInstance()->dbpath, &db);
	if (!status.ok())
	{
		CS_DIE(status.ToString());
	}

	leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
	for (it->SeekToFirst(); it->Valid(); it->Next())
	{
		Word word = it->key().ToString();
		CS_DUMP(word);
		map[word] = DocWeightList();
		unpack(it->value(), map[word]);
		CS_DUMP(map[word][0].docid);
		CS_DUMP(map[word][0].weight);
	}
	delete it;
	delete db;
}

void Index::unpack(const leveldb::Slice& value, DocWeightList& list)
{
	CS_DUMP(value.size());
	msgpack::unpacker unpacker;
	memcpy(unpacker.buffer(), value.data(), value.size());
	unpacker.buffer_consumed(value.size());
	msgpack::unpacked result;
	unpacker.next(&result);
	result.get().convert(&list);
}

} /* namespace fts */
} /* namespace jebe */
