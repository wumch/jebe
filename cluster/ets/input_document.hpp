
#pragma once

#include "predef.hpp"
#include <string>
#include <string.h>
#include <vector>
#include <msgpack.hpp>

namespace jebe {
namespace cluster {
namespace ets {

// document from input.
class InDocument
{
public:
	static const size_t _id_max_size = 32;
	static const size_t url_max_size = 1024;
	static const size_t text_max_size = (126 << 10);

	char text[text_max_size];
	char _id[_id_max_size];		// usually, it's mongodb's ObjectID.
	char url[url_max_size];		// for check the results by hand.

	uint32_t _id_size;
	uint32_t url_size;
	uint32_t text_size;

public:
	InDocument()
		: _id_size(0), url_size(0), text_size(0)
	{}

	explicit InDocument(const std::string& _id_, const std::string& url_, const std::string& text_)
		: _id_size(std::min(_id_.size(), _id_max_size)),
		  url_size(std::min(url_.size(), url_max_size)),
		  text_size(std::min(text_.size(), text_max_size))
	{
		memcpy(_id, _id_.data(), _id_size);
		memcpy(url, url_.data(), url_size);
		memcpy(text, text_.data(), text_size);
	}

	explicit InDocument(const char* _id_, const char* url_, const char* text_)
		: _id_size(std::min(strlen(_id_), _id_max_size)),
		  url_size(std::min(strlen(url_), url_max_size)),
		  text_size(std::min(strlen(text_), text_max_size))
	{
		memcpy(_id, _id_, _id_size);
		memcpy(url, url_, url_size);
		memcpy(text, text_, text_size);
	}

	explicit InDocument(
		const char* _id_, size_t _id_size_,
		const char* url_, size_t url_size_,
		const char* text_, size_t text_size_
	)
		: _id_size(std::min(strlen(_id_), _id_max_size)),
		  url_size(std::min(strlen(url_), url_max_size)),
		  text_size(std::min(strlen(text_), text_max_size))
	{
		memcpy(_id, _id_, _id_size);
		memcpy(url, url_, url_size);
		memcpy(text, text_, text_size);
	}

	operator bool() const
	{
		return _id_size;
	}

private:		// disable stack constructing.
	~InDocument();
};

// for msgpack/unpck.
template<typename key_t, typename val_t>
class KeyValPair
{
public:
	key_t key;
	val_t val;

public:
	KeyValPair() {}

	explicit KeyValPair(key_t key_, val_t count_)
		: key(key_),val(count_)
	{}

	MSGPACK_DEFINE(key, val);
};

// document for output (to).
class OutDocument
{
public:
	typedef KeyValPair<wordid_t, weight_t> WordWeight;
	typedef std::vector<WordWeight> WordWeightList;
	docid_t id;
	WordWeightList words;

public:
	OutDocument()
		: id(0)
	{}

	explicit OutDocument(docid_t id_, const WordWeightList& ww)
		: id(id_), words(ww)
	{}

	operator bool() const
	{
		return id;
	}

	MSGPACK_DEFINE(id, words);
};

} /* namespace ets */
} /* namespace cluster */
} /* namespace jebe */
