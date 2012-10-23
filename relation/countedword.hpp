
#pragma once

#include "predef.hpp"
#include <msgpack.hpp>

namespace jebe {
namespace rel {

// for msgpack/unpck.
class CountedWord
{
public:
	Word word;
	atimes_t count;

public:
	CountedWord() {}

	explicit CountedWord(Word word_, atimes_t atime)
		: word(word_), count(atime)
	{
	}

	MSGPACK_DEFINE(word, count);
};

}
}
