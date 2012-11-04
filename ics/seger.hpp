
#pragma once

#include "predef.hpp"
#include <iconv.h>
#include <boost/static_assert.hpp>
#include <boost/bind.hpp>
#include <glog/logging.h>
#include "utility.hpp"
#include "result.hpp"
#include "dictionary.hpp"
#include "config.hpp"

namespace jebe {
namespace ics {

CS_FORCE_INLINE static void conv(iconv_t cd, char** in, size_t* in_len, char** out, size_t *out_len)
{
	if (CS_BUNLIKELY(iconv(cd, in, in_len, out, out_len) == static_cast<size_t>(-1)))
	{
		LOG(ERROR) << "iconv(in, " << *in_len << ", out, " << *out_len << ") failed!";
	}
}

class WordPOSCal
{
private:
	static const int wpos_end_val = 31233;
	static WordPOS wpos[wpos_end_val];
	static WordPOS empty_wpos;
	static weight_t weights[USHRT_MAX];

public:
	CS_FORCE_INLINE static const WordPOS& getWordPOS(uint16_t nHandle)
	{
		return CS_BLIKELY(nHandle < wpos_end_val) ? wpos[nHandle] : empty_wpos;
	}

	static void init();

	static void assignWeight(uint8_t main, uint8_t sub, weight_t weight_);
	static void assignWeight(uint8_t main, weight_t weight_);

	static weight_t weight(const WordPOS& wp)
	{
		return weights[static_cast<uint16_t>((wp.main << 8) + wp.sub)];
	}
};

template<typename Callback>
class ConvOut
{
	typedef Callback CBType;
private:
	static const int word_iconv_buf_size = 128;

	iconv_t& cd;
	CBType& callback;
	size_t word_iconv_buf_pos;
	char word_iconv_buf[word_iconv_buf_size];
	char* word_iconv_buf_cur;

public:
	ConvOut(iconv_t& cd_, CBType& callback_)
		: cd(cd_), callback(callback_),
		  word_iconv_buf_pos(word_iconv_buf_size),
		  word_iconv_buf_cur(word_iconv_buf)
	{}

	void operator()(PWORD_RESULT words, bool firstWordIgnore)
	{
		for (int i = firstWordIgnore; words[i].sWord[0] != 0 && words[i].nHandle != CT_SENTENCE_END; ++i)
		{
			handleWord(words[i].sWord, strlen(words[i].sWord), WordPOSCal::getWordPOS(words[i].nHandle));
		}
	}

private:
	void handleWord(char* str, size_t str_len, const WordPOS& type)
	{
		word_iconv_buf_pos = word_iconv_buf_size;
		word_iconv_buf_cur = word_iconv_buf;
		conv(cd, &str, &str_len, &word_iconv_buf_cur, &word_iconv_buf_pos);
		callback(word_iconv_buf, word_iconv_buf_cur - word_iconv_buf, type);
	}

};

class Seger
{
protected:
	const char* internal_charset;
	const char* external_charset;

private:
	CResult icter;

	iconv_t in_iconv;
	iconv_t out_iconv;

	char* in_iconv_buf;
	char* in_iconv_buf_cur;
	size_t in_iconv_buf_pos;
	size_t in_iconv_buf_size;

public:
	Seger();

	template<typename Callback>
	void process(char* content, size_t content_len, Callback& callback)
	{
		conv_in(content, content_len);
		BOOST_STATIC_ASSERT(sizeof(typeof(*in_iconv_buf)) == 1);
		ConvOut<Callback> co(out_iconv, callback);
		icter.ParagraphWalk(in_iconv_buf, in_iconv_buf_cur - in_iconv_buf, co);
	}

	~Seger();

protected:
	bool prepare();

	CS_FORCE_INLINE void conv_in(char* content, size_t content_len)
	{
		in_iconv_buf_cur = in_iconv_buf;
		in_iconv_buf_pos = in_iconv_buf_size;
		conv(in_iconv, &content, &content_len, &in_iconv_buf_cur, &in_iconv_buf_pos);
	}
};

} /* namespace ics */
} /* namespace jebe */
