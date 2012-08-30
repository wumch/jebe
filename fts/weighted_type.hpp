
#pragma once

#include <msgpack.hpp>
#include <msgpack/object.hpp>

namespace jebe {
namespace fts {

typedef std::string Word;
typedef uint32_t docid_t;

typedef long double marve_t;
typedef double weight_t;

class WordWeight
{
public:
	Word word;
	weight_t weight;

public:
	WordWeight() {}

	explicit WordWeight(const Word& word_, weight_t weight_)
		: word(word_), weight(weight_)
	{
	}

	MSGPACK_DEFINE(word, weight);
};

class DocWeight
{
public:
	docid_t docid;
	weight_t weight;

public:
	DocWeight() {}

	explicit DocWeight(const docid_t& docid_, weight_t weight_)
		: docid(docid_), weight(weight_)
	{
	}

	MSGPACK_DEFINE(docid, weight);
};

}
}
