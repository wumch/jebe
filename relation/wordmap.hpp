
#pragma once

#include "predef.hpp"
#include <boost/unordered_map.hpp>
#include "autoincr.hpp"

namespace jebe {
namespace rel {

class WordMap
{
public:
	// TODO: optimized-hash and memory-pool.
	typedef boost::unordered_map<Word, wordid_t> WordIdMap;

private:
	class WordIdTag {};
	typedef staging::AutoIncr<WordIdTag, wordid_t, 0> WordIdGen;

	WordIdGen& idGen;

public:
	WordIdMap map;		// {word:id} map.

public:
	WordMap()
		: idGen(*WordIdGen::instance())
	{
	}

	wordid_t getWordId(const Word& word)
	{
		WordIdMap::iterator it = map.find(word);
		return CS_BLIKELY(it != map.end()) ?
			it->second : 0;
	}

	wordid_t attachWord(const Word& word)
	{
		map.insert(std::make_pair(word, idGen.gen()));
		return idGen.last();
	}
};

} /* namespace rel */
} /* namespace jebe */
