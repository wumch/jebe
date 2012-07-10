
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include "../../staging/urlcode.hpp"

int main(int argc, char* argv[])
{
	boost::filesystem::path contentFile;
	uint64_t max_chars;
	switch (argc)
	{
	case 2:
		contentFile = argv[1];
		break;
	case 3:
		contentFile = argv[1];
		max_chars = boost::lexical_cast<uint64_t>(argv[2]);
		break;
	default:
		CS_DIE("useage: " << argv[0] << " content-file" << " max-chars");
		break;
	}
}
