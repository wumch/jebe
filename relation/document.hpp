
#pragma once

#include "predef.hpp"
#include <string>
#include <vector>
#include <boost/noncopyable.hpp>
#include <msgpack.hpp>
#include "countedword.hpp"

namespace jebe {
namespace rel {

class Document
	: private boost::noncopyable
{
public:
	typedef std::vector<CountedWord> WordList;

	const docid_t id;
	WordList words;

public:
	explicit Document(docid_t id_, const WordList& words_)
		: id(id_), words(words_)
	{
	}

	// consturct a document from a (usually received via socket) binarry buffer.
	// just to avoid from memory-copy.
	explicit Document(docid_t id_, const char* buf, size_t len)
		: id(id_)
	{
		msgpack::unpacked msg;
		try
		{
			msgpack::unpack(&msg, buf, len);
			msg.get().convert(&words);
		}
		catch (const std::exception& e)
		{
			CS_DIE("kid, error occured: " << e.what());
		}
	}
};

static inline bool operator==(const Document& doc_1, const Document& doc_2)
{
	return &doc_1.id == &doc_2.id;
}

} /* namespace rel */
} /* namespace jebe */
