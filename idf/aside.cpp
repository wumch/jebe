
#include "aside.hpp"
#include <fstream>
#include <iostream>
#include <boost/date_time/local_time/local_time.hpp>
#include <glog/logging.h>
#include "datetime.hpp"
#include "config.hpp"
#include "calculater.hpp"

#define __JEBE_PATTEN_LINE_SIZE_MAX	256

namespace jebe {
namespace idf {

Aside::WordList Aside::wordList;
Ftree* const Aside::ftree = new Ftree;
Filter* const Aside::filter = new Filter;

const Config* const Aside::config = Config::getInstance();

docnum_t Aside::curDocNum = 0;
docnum_t Aside::totalDocNum = 0;

void Aside::init()
{
	initLogger();
	initWordList();
}

void Aside::initWordList()
{
	std::ifstream pfile(config->pattenfile.string().c_str());
	std::ifstream::pos_type last_pos = 0;
	size_t line_len;
	char line[__JEBE_PATTEN_LINE_SIZE_MAX];
	while (pfile.good() && static_cast<int>(pfile.peek()) != -1)
	{
		pfile.getline(line, __JEBE_PATTEN_LINE_SIZE_MAX);
		line_len = pfile.tellg() - last_pos - 1;
		if (CS_BUNLIKELY(line_len >= __JEBE_PATTEN_LINE_SIZE_MAX))
		{
			CS_DIE("pattenfile.getline() not really reach line end.");
		}
		attachWord(Word(line, line_len));
		last_pos = pfile.tellg();
	}
	filter->attachTree(ftree);
	Calculater::init();
}

void Aside::initLogger()
{
	google::InitGoogleLogging(config->argv_first);
	google::SetLogDestination(google::INFO, config->logfile.string().c_str());
}

wordid_t Aside::attachWord(const Word& word)
{
	wordList.push_back(word);
	ftree->attach_word(word, wordList.size() - 1);
	return wordList.size() - 1;
}

wordnum_t Aside::wordsNum()
{
	return wordList.size();
}

} /* namespace rel */
} /* namespace jebe */

#undef __JEBE_PATTEN_LINE_SIZE_MAX
