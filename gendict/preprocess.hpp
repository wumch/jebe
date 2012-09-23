
#include "ppredef.hpp"
#include <iostream>
#include <fstream>
#include <boost/version.hpp>
#include "config.hpp"
#include "filter.hpp"

namespace jebe {
namespace cws {

class Preprocess
{
protected:
	Filter* filter;

public:
	Preprocess()
		: filter(NULL)
	{
	}

	void run()
	{
		prepare();
		process();
		dump();
	}

protected:
	void prepare()
	{
#if BOOST_VERSION > 104200
		filter = new Filter(Config::getInstance()->pattenfile.string());
#else
		filter = new Filter(Config::getInstance()->pattenfile);
#endif
	}

	void process()
	{
		scan();
	}

	void dump()
	{
		filter->dumpTree();
	}

	void scan()
	{
		char* const content = new char[_JEBE_PROCESS_STEP];

		const Config* config = Config::getInstance();
		for (Config::FileList::const_iterator it = config->inputfiles.begin(); it != config->inputfiles.end(); ++it)
		{
			std::fstream file(it->c_str(), std::ios_base::in);
			file.imbue(std::locale(""));

			ssize_t readed = 0, remains = 0;
			while (true)
			{
				if (CS_BUNLIKELY((readed = file.readsome(content + remains, _JEBE_PROCESS_STEP - remains)) <= 0))
				{
					break;
				}
				remains = filter->find(reinterpret_cast<byte_t*>(content), readed + remains);
				if (CS_BUNLIKELY(remains > (_JEBE_PROCESS_STEP >> 1)))
				{
					CS_DIE("(remains > (_JEBE_PROCESS_STEP + 1) >> 1)");
				}
				memcpy(content, content + (_JEBE_PROCESS_STEP - remains), remains);
			}

			file.close();
			std::cout << CS_OC_GREEN(*it << " done") << std::endl;
		}
		delete[] content;
	}
};

}
}
