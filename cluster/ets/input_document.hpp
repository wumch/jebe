
#pragma once

#include "predef.hpp"
#include <string>
#include <string.h>
#include <vector>
#include <msgpack.hpp>
#include "config.hpp"
#include "aside.hpp"

namespace jebe {
namespace cluster {
namespace ets {

// document from input.
class InDocument
{
public:
	static const uint32_t _id_max_size = 32;
	static const uint32_t url_max_size = 1024;
	static const uint32_t text_max_size = (126 << 10);
	static const uint32_t title_max_size = 128;

	char text[text_max_size];
	char _id[_id_max_size];		// usually, it's mongodb's ObjectID.
	char url[url_max_size];		// for check the results by hand.

	uint32_t _id_size;
	uint32_t url_size;
	uint32_t text_size;
	uint32_t title_size;

public:
	InDocument()
		: _id_size(0), url_size(0), text_size(0), title_size(0)
	{}

	explicit InDocument(const std::string& _id_, const std::string& url_, const std::string& text_, const std::string& title_ = std::string())
		: _id_size(std::min<uint32_t>(_id_.size(), _id_max_size)),
		  url_size(std::min<uint32_t>(url_.size(), url_max_size)),
		  text_size(std::min<uint32_t>(text_.size(), text_max_size)),
		  title_size(std::min<uint32_t>(title_.size(), title_max_size))
	{
		memcpy(_id, _id_.data(), _id_size);
		memcpy(url, url_.data(), url_size);
		gen_text(title_.data(), text_.data());
	}

	explicit InDocument(const char* _id_, const char* url_, const char* text_, const char* title_ = NULL)
		: _id_size(std::min<uint32_t>(strlen(_id_), _id_max_size)),
		  url_size(std::min<uint32_t>(strlen(url_), url_max_size)),
		  text_size(std::min<uint32_t>(strlen(text_), text_max_size)),
		  title_size((title_ == NULL) ? 0 : (std::min<uint32_t>(strlen(title_), title_max_size)))
	{
		memcpy(_id, _id_, _id_size);
		memcpy(url, url_, url_size);
		gen_text(title_, text_);
	}

	explicit InDocument(
		const char* _id_, size_t _id_size_,
		const char* url_, size_t url_size_,
		const char* text_, size_t text_size_,
		const char* title_ = NULL, size_t title_size_ = 0
	)
		: _id_size(std::min<uint32_t>(strlen(_id_), _id_max_size)),
		  url_size(std::min<uint32_t>(strlen(url_), url_max_size)),
		  text_size(std::min<uint32_t>(strlen(text_), text_max_size)),
		  title_size(std::min<uint32_t>(strlen(title_), title_max_size))
	{
		memcpy(_id, _id_, _id_size);
		memcpy(url, url_, url_size);
		gen_text(title_, text_);
	}

	operator bool() const
	{
		return _id_size;
	}

private:		// disable stack constructing.
	~InDocument();

	void gen_text(const char* title_, const char* text_)
	{
		uint32_t offset = 0;
		if (title_size)
		{
			for (uint i = 0; i < Aside::config->doc_title_weight; ++i)
			{
				memcpy(text + offset, title_, title_size);
				offset += title_size;
			}
		}
		memcpy(text + offset, text_, std::min(text_size, text_max_size - offset));
	}
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
