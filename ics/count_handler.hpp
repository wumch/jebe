
#pragma once

#include "predef.hpp"
#include <utility>
#include <boost/unordered_map.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <msgpack.hpp>
#include "handler.hpp"
#include "node.hpp"
#include "dictionary.hpp"
#include "seger.hpp"
#include "mbswcs.hpp"
#include "config.hpp"

namespace jebe {
namespace ics {

typedef msgpack::type::define<const std::string*, atimes_t> CWordAtimePacker;

class CWordAtime
	: public CWordAtimePacker
{
public:
	const std::string* word;
	atimes_t atimes;

public:
	explicit CWordAtime(const std::string* node_, atimes_t atimes_)
		: CWordAtimePacker(node_, atimes_),
		  word(node_), atimes(atimes_)
	{
	}

	CWordAtime& operator=(const CWordAtime& ww)
	{
		word = ww.word;
		atimes = ww.atimes;
		a0 = ww.a0;
		a1 = ww.a1;
		return *this;
	}

	template <typename Packer>
	void msgpack_pack(Packer& pk) const
	{
		pk.pack_array(2);
		pk.pack(*word);
		pk.pack(atimes);
	}
};

// hold words for calculate the marvelously-score.
class CountHolder
{
private:
	class NodeHash
	{
	private:
		uint32_t mask;
	public:
		uint32_t operator()(const Node* node) const
		{
			return (size_t)(node) & mask;
		}
	};

public:
	typedef boost::unordered_map<std::string, atimes_t> MapShadeType;
	typedef boost::fast_pool_allocator<MapShadeType::value_type,
		boost::default_user_allocator_new_delete,
		boost::details::pool::null_mutex, 1024> WordsAllocType;
	typedef boost::unordered_map<std::string, atimes_t, MapShadeType::hasher, MapShadeType::key_equal, WordsAllocType> Words;

	typedef boost::fast_pool_allocator<CWordAtime,
		boost::default_user_allocator_new_delete,
		boost::details::pool::null_mutex, 1024> OrderedWordsAllocType;
	typedef std::vector<CWordAtime> OrderedWords;

	Words words;

	static std::string empty_str;
	std::pair<std::string, atimes_t> word_atime;		// assit to avoid stack alloc.

	msgpack::sbuffer packerBuffer;
	msgpack::packer<msgpack::sbuffer> packer;

public:
	explicit CountHolder():
		words(128),
		word_atime(empty_str, 1),
		packer(packerBuffer)
	{}

	void operator()(char* word, size_t word_len, const WordPOS& type)
	{
		weight_t weight = WordPOSCal::weight(type);
		if (weight != 0)
		{
			word_atime.first = std::string(word, word_len);
			Words::iterator it = words.find(std::string(word, word_len));
			if (it == words.end())
			{
				word_atime.second = 1;
				words.insert(word_atime);
			}
			else
			{
				++it->second;
			}
		}
	}

	class MarveCompare
	{
	public:
		bool operator()(const CWordAtime& w1, const CWordAtime& w2) const
		{
			return w1.atimes > w2.atimes;
		}
	};
	MarveCompare comparer;

	void genRes(zmq::message_t& rep)
	{
		std::auto_ptr<OrderedWords> ow(new OrderedWords);
		ow->reserve(words.size());
		for (Words::const_iterator it = words.begin(); it != words.end(); ++it)
		{
			ow->push_back(CWordAtime(&it->first, it->second));
		}
		std::sort(ow->begin(), ow->end(), comparer);

		packerBuffer.clear();
		packer.pack(*ow);
		rep.rebuild(packerBuffer.data(), packerBuffer.size(), NULL, NULL);
	}

	void reset()
	{
		words.clear();
	}
};


class CountHandler
	: public Handler
{
private:
	CountHolder holder;

public:
	CountHandler()
	{}

	virtual HandleRes process(char* content, tsize_t len, zmq::message_t& rep)
	{
		holder.reset();
		seger->process(content, len, holder);
		holder.genRes(rep);
		return success;
	}
};

}
}
