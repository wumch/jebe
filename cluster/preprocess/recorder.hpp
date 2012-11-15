
#pragma once

#include "predef.hpp"
#include <vector>
#include <boost/dynamic_bitset.hpp>
#include "node.hpp"

namespace jebe {
namespace idf {

// record words appear in a certain document, for calculate the df.
class Recorder
{
public:
	typedef std::vector<const Node*> NodeList;

protected:
	boost::dynamic_bitset<uint64_t> flags;
	NodeList nodes;

public:
	explicit Recorder()
		: flags(Aside::wordsNum(), 0)
	{
		nodes.reserve(2 < 10);
	}

	void operator()(const Node* node)
	{
		if (!flags.test(node->pattenid))
		{
			flags.set(node->pattenid, true);
			nodes.push_back(node);
		}
	}

	void reset()
	{
		for (NodeList::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
		{
			flags.reset((*it)->pattenid);
		}
		nodes.clear();
	}

	const NodeList& recordedNodes() const
	{
		return nodes;
	}
};

}
}
