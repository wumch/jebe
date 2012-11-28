
#pragma once

#include "predef.hpp"
#include <vector>
#include <exception>
#include <string.h>
#include <boost/pool/pool_alloc.hpp>
#include <boost/unordered_map.hpp>
#include "hash.hpp"
#include "autoincr.hpp"
#include "../document.hpp"

namespace jebe {
namespace cluster {
namespace ets {

class WordNonExists
	: public std::exception
{
private:
	mutable std::string reason;

public:
	Word word;

public:
	WordNonExists(const Word& w)
		: word(w)
	{
	}

	virtual const char* what() const throw()
	{
		if (reason.size() == 0)
		{
			reason = "kid, word [" + word + "] non-exists!";
		}
		return reason.c_str();
	}

	virtual ~WordNonExists() throw()
	{
	}
};

class WordMap
{
private:
	static const wordid_t useless_wordid = static_cast<wordid_t>(-1);

public:
	typedef staging::HashHflp<_JEBE_WORD_MAP_HASH_BITS, Word> WordMapHash;
	typedef boost::fast_pool_allocator<boost::unordered_map<Word, wordid_t>::value_type, boost::default_user_allocator_new_delete,
		boost::details::pool::null_mutex, 1 << _JEBE_WORD_MAP_HASH_BITS> MapAllocType;
	typedef boost::unordered_map<Word, wordid_t, WordMapHash, std::equal_to<std::string>, MapAllocType> WordIdMap;

	typedef staging::HashHflp<10, Word> SynWordMapHash;
	typedef std::list<Word> SynWordList;
	typedef boost::fast_pool_allocator<boost::unordered_map<Word, SynWordList>::value_type, boost::default_user_allocator_new_delete,
		boost::details::pool::null_mutex, 20 << 10> SynMapAllocType;
	typedef boost::unordered_map<Word, SynWordList, SynWordMapHash, std::equal_to<std::string>, SynMapAllocType> SynWordIdMap;

	typedef std::vector<weight_t> IDFMap;

private:
	class WordIdTag {};
	typedef staging::AutoIncr<WordIdTag, wordid_t, 0> WordIdGen;

	WordIdGen* idGen;

public:
	WordIdMap map;		// {word:id} map.
	SynWordIdMap synmap;
	IDFMap idfmap;

public:
	WordMap()
		: idGen(WordIdGen::instance()), map(1 << _JEBE_WORD_MAP_HASH_BITS)
	{}

	bool wordExists(const Word& word) const throw()
	{
		return map.find(word) != map.end();
	}

	wordid_t getWordId(const Word& word) throw(WordNonExists)
	{
		WordIdMap::iterator it = map.find(word);
		if (CS_BUNLIKELY(it == map.end()))
		{
			CS_SAY("[" << word << "] [" << map.begin()->first << "] map.size() = " << map.size());
			throw WordNonExists(word);
		}
		return it->second;
	}

	weight_t getIDFById(const wordid_t wordid) const
	{
		return idfmap[wordid];
	}

	wordid_t attachWord(const Word& word, weight_t weight)
	{
		wordid_t wid = getSynWordId(word);
		if (wid == useless_wordid)
		{
			wid = idGen->gen();
		}
		map.insert(std::make_pair(word, wid));
		if (idfmap.size() <= wid)
		{
			idfmap.resize(wid + 1);
		}
		idfmap[wid] = weight;
		return wid;
	}

	wordid_t operator[](const Word& word)
	{
		return getWordId(word);
	}

	void build_finished();

	wordnum_t size() const
	{
		return map.size();
	}

	void build_synonym_map(const std::string& file);

private:
	void dump();

	wordid_t getSynWordId(const Word& word)
	{
		WordIdMap::iterator mit = map.find(word);
		if (CS_BUNLIKELY(mit != map.end()))
		{
			return mit->second;
		}
		SynWordIdMap::iterator it = synmap.find(word);
		if (it != synmap.end())
		{
			const SynWordList& slist = it->second;
			for (SynWordList::const_iterator sit = slist.begin(); sit != slist.end(); ++sit)
			{
				mit = map.find(*sit);
				if (mit != map.end())
				{
					return mit->second;
				}
			}
		}
		return useless_wordid;
	}

	void attachSyn(const Word& word, const Word& syn);
};

} /* namespace ets */
} /* namespace cluster */
} /* namespace jebe */
