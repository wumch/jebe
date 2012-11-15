
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
namespace cluster {

WordMap Aside::wordmap;

const Config* const Aside::config = Config::getInstance();
Calculater* const Aside::caler = new Calculater;

docnum_t Aside::curDocNum = 0;
docnum_t Aside::totalDocNum = 0;

void Aside::init()
{
	initLogger();
	initWordMap();
	initCalculater();
}

void Aside::initWordMap()
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
		wordmap.attachWord(Word(line, line_len));
		last_pos = pfile.tellg();
	}
}

void Aside::initLogger()
{
	google::InitGoogleLogging(config->argv_first);
	google::SetLogDestination(google::INFO, config->logfile.string().c_str());
//	google::SetLogFilenameExtension("log");
//	setenv("GLOG_log_dir", (config->logfile.string() + " " + config->argv_first).c_str(), 0);
}

void Aside::initCalculater()
{
	caler->prepare();
}

} /* namespace cluster */
} /* namespace jebe */

#undef __JEBE_PATTEN_LINE_SIZE_MAX
