
#include "index.hpp"

namespace jebe {
namespace fts {

const Index* const Index::instance = new Index;

Index::Index()
{
}

void Index::match(const WordWeightList& words, std::vector<docid_t>& docids, std::size_t max_match) const
{
	dwmap.clear();

	for (WordWeightList::const_iterator wit = words.begin(); wit != words.end(); ++wit)
	{
		MapType::const_iterator entry = map.find(wit->word);
		if (entry != map.end())
		{
			const DocWeightList& docs = entry->second;
			for (DocWeightList::const_iterator dit = docs.begin(); dit != docs.end(); ++dit)
			{
				DocWeightMap::iterator doc = dwmap.find(dit->docid);
				if (doc == dwmap.end())
				{
					dwmap[dit->docid] = dit->weight * wit->weight;
				}
				else
				{
					doc->second += dit->weight * wit->weight;
				}
			}

			if (CS_BUNLIKELY(!(dwmap.size() < Config::getInstance()->max_match)))
			{
				break;
			}
		}
	}

	// TODO: currently find the most high one.
	marve_t min_marve = .0;
	for (DocWeightMap::const_iterator it = dwmap.begin(); it != dwmap.end(); ++it)
	{
		if (it->second >= min_marve)
		{
			docids[0] = it->first;
			min_marve = it->second;
		}
	}
}

} /* namespace fts */
} /* namespace jebe */
