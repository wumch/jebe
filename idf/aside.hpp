
#pragma once

#include "predef.hpp"
#include <vector>

namespace jebe {
namespace idf {

class Calculater;
class Config;
class Ftree;
class Filter;

enum Action {
	send_doc = 1,
	thats_all = 10,
};

// misc class that hold some global-variables.
class Aside
{
public:
	typedef std::vector<Word> WordList;
	static WordList wordList;

	static Ftree* const ftree;
	static Filter* const filter;

	static const Config* const config;

	static docnum_t totalDocNum;
	static docnum_t curDocNum;		// current analysised document count.

	static void init();

protected:
	static void initLogger();

	static void initWordList();

public:
	static wordnum_t wordsNum();

	static wordid_t attachWord(const Word& word);
};

} /* namespace rel */
} /* namespace jebe */
