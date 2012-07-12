
#include "staging.hpp"
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include "mbswcs.hpp"
#include "extractor.hpp"

int main(int argc, char* argv[])
{
	boost::filesystem::path contentfile, gbfile, outfile;
	uint64_t maxchars = 0;

	if (argc < 4)
	{
		CS_DIE("useage: " << argv[0] <<
			" gb2312-chars-file"
			" content-file"
			" words-output-file"
			" max-chars"
		);
	}

	gbfile = argv[1];
	contentfile = argv[2];
	outfile = argv[3];
	if (argc >= 5)
	{
		maxchars = boost::lexical_cast<uint64_t>(argv[4]);
	}

	staging::mbswcs::setlocale();
	jebe::cws::Extractor extractor(gbfile);
	extractor.extract(contentfile, outfile, maxchars);
}
