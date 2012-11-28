
#pragma once

#include "predef.hpp"

namespace jebe {
namespace cluster {

class RConfig;

namespace ets {

class Config;
class Ftree;
class Filter;
class WordMap;
class Transfer;

// misc class that hold some global-variables.
class Aside
{
public:
	static WordMap* const wordmap;

	static Ftree* const ftree;
	static Filter* const filter;

	static Transfer* const transfer;

	static const Config* const config;
	static RConfig* const rconfig;

	static docnum_t totalDocNum;
	static docnum_t curDocNum;		// current analysised document count.

	static void init(int argc, char* argv[]);

protected:
	static void initConfig(int argc, char* argv[]);

	static void initLogger();

	static void initWordList();

public:
	static wordnum_t wordsNum();

	static wordid_t attachWord(const Word& word, weight_t idf);
};

} /* namespace ets */
} /* namespace cluster */
} /* namespace jebe */
