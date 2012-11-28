
#include "wordmap.hpp"
#include <fstream>
#include "aside.hpp"
#include "config.hpp"

namespace jebe {
namespace cluster {
namespace ets {

void WordMap::build_finished()
{
	dump();
	synmap.clear();
}

void WordMap::dump()
{
	std::ofstream ofile(Aside::config->wordid_outputfile.string().c_str());
	for (WordIdMap::const_iterator it = map.begin(); it != map.end(); ++it)
	{
		ofile << it->first << Aside::config->output_delimiter << it->second << Aside::config->output_delimiter << idfmap[it->second] << CS_LINESEP_STR;
	}
	ofile.close();
}

void WordMap::build_synonym_map(const std::string& file)
{
	FILE* fp = fopen(file.c_str(), "r");
	char word[__JEBE_PATTEN_LINE_SIZE_MAX];
	char syn[__JEBE_PATTEN_LINE_SIZE_MAX];
	while (!feof(fp))
	{
		memset(word, 0, __JEBE_PATTEN_LINE_SIZE_MAX);
		memset(syn, 0, __JEBE_PATTEN_LINE_SIZE_MAX);
		if (CS_BUNLIKELY(fscanf(fp, "%s\t%s\n", word, syn) != 2))
		{
			CS_DIE("kid, IO error occured, fscanf(" << file << ", \"%s\t%s\n\", word, syn) != 2");
		}
		Word w(word), s(syn);
		attachSyn(w, s);
		attachSyn(s, w);
	}
}

void WordMap::attachSyn(const Word& word, const Word& syn)
{
	SynWordIdMap::iterator it = synmap.find(word);
	if (it != synmap.end())
	{
		for (SynWordList::const_iterator sit = it->second.begin(); sit != it->second.end(); ++sit)
		{
			if (*sit == syn)
			{
				return;
			}
		}
		it->second.push_back(syn);
	}
	else
	{
		SynWordList slist;
		slist.push_back(syn);
		synmap.insert(std::make_pair(word, slist));
	}
}

} /* namespace ets */
} /* namespace cluster */
} /* namespace jebe */
