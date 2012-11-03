
#pragma once

#include "predef.hpp"
#include <iconv.h>

namespace jebe {
namespace ics {

class Segment
{
protected:
	const char* internal_charset;
	const char* external_charset;

private:
	iconv_t in_iconv;
	iconv_t out_iconv;

	char* in_iconv_buf;
	size_t in_iconv_buf_size;

	char* out_iconv_buf;
	size_t out_iconv_buf_size;

	char* seg_buf;
	size_t seg_buf_size;
	size_t seg_out_size;

public:
	Segment();

	void process(char* content, size_t content_len);

	~Segment();

protected:
	bool prepare();

	void conv_in(char* content, size_t content_len);

	void conv_out();

	void conv(iconv_t cd, char** in, size_t* in_len, char** out, size_t *out_len);
};

} /* namespace ics */
} /* namespace jebe */
