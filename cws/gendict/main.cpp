
#include "staging.hpp"
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include "urlcode.hpp"
#include "mbswcs.hpp"
#include "extractor.hpp"

int main(int argc, char* argv[])
{
	boost::filesystem::path contentFile, gbfile;
	uint64_t max_chars = 0;
	switch (argc)
	{
	case 3:
		contentFile = argv[1];
		gbfile = argv[2];
		break;
	case 4:
		contentFile = argv[1];
		gbfile = argv[2];
		max_chars = boost::lexical_cast<uint64_t>(argv[3]);
		break;
	default:
		CS_DIE("useage: " << argv[0] << " content-file gb2312-chars-file" << " max-chars");
		break;
	}

	staging::mbswcs::setlocale();
//	staging::mbswcs::setlocale("zh_CN.UTF-8");
	jebe::cws::Extractor extractor(gbfile);
	extractor.extract(contentFile, max_chars);
	extractor.display();
}
