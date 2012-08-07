
#include "staging.hpp"
#include <iostream>
#include <auto_ptr.h>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include "mbswcs.hpp"
#include "extractor.hpp"

int main(int argc, char* argv[])
{
	using namespace jebe::cws;

	Extractor::PathList contentfiles;
	boost::filesystem::path gbfile, outfile;
	uint64_t maxchars = 0;

	if (argc < 4)
	{
		CS_DIE("useage: " << argv[0] <<
			" <gb2312-chars-file>"
			" <words-output-file>"
			" <content-file> [content-file ...]"
//			" max-chars"
		);
	}

	gbfile = argv[1];
	outfile = argv[2];

	for (int i = 3; i < argc; ++i)
	{
		contentfiles.push_back(argv[i]);
	}
//	if (argc >= 5)
//	{
//		maxchars = boost::lexical_cast<uint64_t>(argv[4]);
//	}

	staging::mbswcs::setlocale();
	std::auto_ptr<Extractor> extractor(new Extractor(gbfile));
	extractor->extract(contentfiles, outfile, maxchars);
}
