
#pragma once

#include "predef.hpp"
#include <iostream>
#include <fstream>
#include <boost/unordered_map.hpp>
#include "datetime.hpp"
#include "wordmap.hpp"

namespace jebe {
namespace rel {

class Calculater;
class Config;

class Logger
	: public std::ofstream
{
public:
	Logger(const char* file, std::ios_base::openmode mode = std::ios_base::out|std::ios_base::trunc)
		: std::ofstream(file, mode)
	{
//		*this << "begin at " << staging::datetime() << CS_LINESEP;
	}

	~Logger()
	{
		flush();
	}
};

// misc class that hold some global-variables.
class Aside
{
public:
	static WordMap wordmap;

	static const Config* const config;

	static docnum_t totalDocNum;
	static docnum_t curDocNum;		// current analysised document count.

	static Calculater* const caler;

	static void init();

protected:
	static void initLogger();

	static void initWordMap();

	static void initCalculater();
};

} /* namespace rel */
} /* namespace jebe */
