
#pragma once

#include "predef.hpp"
#include <utility>
#include <boost/unordered_map.hpp>
#include <msgpack.hpp>
#include "hash.hpp"
#include "config.hpp"
#include "wordmap.hpp"
#include "node.hpp"
#include "../document.hpp"

namespace jebe {
namespace cluster {
namespace ets {

// hold words for calculate the marvelously-score.
class MarveHolder
{
public:
	typedef staging::HashTomasWang32<fid_t> FidHash;
	typedef boost::unordered_map<fid_t, atimes_t, FidHash> Words;
	Words words;
	tsize_t words_atimes_total;

	std::pair<fid_t, atimes_t> word_atime;		// assit to avoid stack alloc.

public:
	explicit MarveHolder():
		words(256), words_atimes_total(0), word_atime(0, 1)
	{
	}

	void operator()(const Node* node)
	{
		Words::iterator it = words.find(node->pattenid);
		if (CS_BUNLIKELY(it == words.end()))
		{
			word_atime.first = node->pattenid;
			words.insert(word_atime);
		}
		else
		{
			it->second++;
		}
		++words_atimes_total;
	}

	class MarveCompare
	{
	public:
		bool operator()(const Feature& w1, const Feature& w2) const
		{
			return w1.fval > w2.fval;
		}
	};
	MarveCompare comparer;

	void genRes(FeatureList& flist)
	{
		flist.reserve(words.size());
		for (Words::const_iterator it = words.begin(); it != words.end(); ++it)
		{
			flist.push_back(Feature(it->first, (it->second * Aside::wordmap->getIDFById(it->first) / words_atimes_total)));
		}
		std::sort(flist.begin(), flist.end(), comparer);
		if (flist.size() > Aside::config->doc_top_words)
		{
			flist.resize(Aside::config->doc_top_words);
		}
	}

	void reset()
	{
		words.clear();
		words_atimes_total = 0;
	}
};

} /* namespace ets */
} /* namespace cluster */
} /* namespace jebe */
