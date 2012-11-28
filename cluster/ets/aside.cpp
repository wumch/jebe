
#include "aside.hpp"
#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <boost/lexical_cast.hpp>
#include <glog/logging.h>
#include "misc.hpp"
#include "config.hpp"
#include "wordmap.hpp"
#include "filter.hpp"
#include "transfer.hpp"
#include "../rconfig.hpp"

namespace jebe {
namespace cluster {
namespace ets {

WordMap* const Aside::wordmap = new WordMap;
Ftree* const Aside::ftree = new Ftree;
Filter* const Aside::filter = new Filter;
const Config* const Aside::config = Config::getInstance();
RConfig* const Aside::rconfig = RConfig::getInstance();
Transfer* const Aside::transfer = new Transfer;

docnum_t Aside::curDocNum = 0;
docnum_t Aside::totalDocNum = 0;

void Aside::init(int argc, char* argv[])
{
	initConfig(argc, argv);
	initLogger();
	initWordList();
}

void Aside::initConfig(int argc, char* argv[])
{
	Config::getInst()->init(argc, argv);
}

void Aside::initWordList()
{
	wordmap->build_synonym_map(Aside::config->synonymfile.string());
	FILE* pfile = fopen(config->pattenfile.string().c_str(), "r");
	weight_t idf;
	char line[__JEBE_PATTEN_LINE_SIZE_MAX];
	while (!feof(pfile))
	{
		memset(line, 0, __JEBE_PATTEN_LINE_SIZE_MAX);
		if (CS_BUNLIKELY(fscanf(pfile, "%s\t%lf\n", line, &idf) != 2))
		{
			CS_DIE("kid, fileds got from fscanf(" << Aside::config->synonymfile << ") is not 2.");
		}
		attachWord(Word(line), idf);
	}
	filter->attachTree(ftree);
	LOG_IF(INFO, config->loglevel > 0) << config->prelog.str();
	wordmap->build_finished();
}

void Aside::initLogger()
{
	google::InitGoogleLogging(config->argv_first);
	google::SetLogDestination(google::INFO, config->logfile.string().c_str());
}

wordid_t Aside::attachWord(const Word& word, weight_t idf)
{
	wordid_t wordid = wordmap->attachWord(word, idf);
	ftree->attach_word(word, wordid);
	return wordid;
}

wordnum_t Aside::wordsNum()
{
	return wordmap->size();
}

} /* namespace ets */
} /* namespace cluster */
} /* namespace jebe */

#undef __JEBE_PATTEN_LINE_SIZE_MAX
