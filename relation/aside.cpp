
#include "aside.hpp"
#include <fstream>
#include <iostream>
#include "config.hpp"
#include "calculater.hpp"

#define __JEBE_PATTEN_LINE_SIZE_MAX	256

namespace jebe {
namespace rel {

WordMap Aside::wordmap;

const Config* const Aside::config = Config::getInstance();
Calculater* const Aside::caler = new Calculater;

docnum_t Aside::curDocNum = 0;
docnum_t Aside::totalDocNum = 0;

void Aside::init()
{
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

void Aside::initCalculater()
{
	caler->prepare();
}

} /* namespace rel */
} /* namespace jebe */

#undef __JEBE_PATTEN_LINE_SIZE_MAX
