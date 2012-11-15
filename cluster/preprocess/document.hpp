
#pragma once

#include "predef.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <boost/noncopyable.hpp>
#include <msgpack.hpp>
#include "countedword.hpp"
#include <ICTCLAS50.h>

namespace jebe {
namespace cluster {

class Document
	: private boost::noncopyable
{
private:
	static size_t moffset;
	static msgpack::object mobj;
	static msgpack::zone mzone;

public:
	typedef std::vector<CountedWord> WordList;

	const docid_t id;
	WordList words;

public:
	explicit Document(docid_t id_, const WordList& words_)
		: id(id_), words(words_)
	{}

	// consturct a document from a (usually received via socket) binarry buffer.
	// just to avoid from memory-copy.
	explicit Document(docid_t id_, const char* buf, size_t len)
		: id(id_)
	{
		moffset = 0;
		if (CS_BLIKELY(msgpack::unpack(buf, len, &moffset, &mzone, &mobj) == msgpack::UNPACK_SUCCESS))
		{
			mobj.convert(&words);
		}
		else
		{
			CS_DIE("kid, unpack <Document(" << id << ")> from " << len << " bytes failed!");
		}
	}

public:
	static void finalize()
	{
		mzone.~zone();
	}
};

static inline bool operator==(const Document& doc_1, const Document& doc_2)
{
	return &doc_1.id == &doc_2.id;
}

} /* namespace cluster */
} /* namespace jebe */
