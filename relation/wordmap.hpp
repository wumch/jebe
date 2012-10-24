
#pragma once

#include "predef.hpp"
#include <exception>
#include <string.h>
#include <boost/unordered_map.hpp>
#include "hash.hpp"
#include "autoincr.hpp"

namespace jebe {
namespace rel {

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
public:
	typedef staging::Hflp<_JEBE_WORD_MAP_HASH_BITS, Word> WordMapHash;
	typedef boost::unordered_map<Word, wordid_t, WordMapHash, std::equal_to<std::string> > WordIdMap;

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

	bool wordExists(const Word& word) const
	{
		return map.find(word) != map.end();
	}

	wordid_t getWordId(const Word& word)
	{
		WordIdMap::iterator it = map.find(word);
		if (CS_BUNLIKELY(it == map.end()))
		{
			CS_SAY("[" << word << "] [" << map.begin()->first << "] map.size() = " << map.size());
			throw WordNonExists(word);
		}
		return it->second;
	}

	wordid_t attachWord(const Word& word)
	{
		map.insert(std::make_pair(word, idGen.gen()));
		CS_SAY("insert word [" << word << "] (" << map[word] << ")");
		return idGen.last();
	}
};

} /* namespace rel */
} /* namespace jebe */
