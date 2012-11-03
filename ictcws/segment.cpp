
#include "segment.hpp"
#include <glog/logging.h>
#include "config.hpp"

namespace jebe {
namespace ics {

Segment::Segment()
{

}

void Segment::process(char* content, size_t content_len)
{
	conv_in(content, content_len);
	seg_out_size = 0;

	conv_out();
}


void Segment::conv_in(char* content, size_t content_len)
{
	conv(in_iconv, &content, &content_len, &in_iconv_buf, &in_iconv_buf_size);
}

void Segment::conv_out()
{
	conv(out_iconv, &seg_buf, &seg_out_size, &out_iconv_buf, &out_iconv_buf_size);
}

void Segment::conv(iconv_t cd, char** in, size_t* in_len, char** out, size_t *out_len)
{
	if (CS_BUNLIKELY(iconv(in_iconv, in, in_len, out, out_len) != static_cast<size_t>(-1)))
	{
		LOG(ERROR) << "iconv(in, " << *in_len << ", out, " << *out_len << ") failed!";
	}
}

bool Segment::prepare()
{
	const Config* const config = Config::getInstance();

	internal_charset = config->internal_charset.c_str();
	external_charset = config->external_charset.c_str();

	in_iconv = iconv_open(internal_charset, external_charset);
	out_iconv= iconv_open(external_charset, internal_charset);
	CS_RETURN_IF(in_iconv == NULL || out_iconv == NULL, false);

	in_iconv_buf_size = config->iconv_buf_size;
	in_iconv_buf = new char[in_iconv_buf_size];
	out_iconv_buf_size = config->iconv_buf_size;
	out_iconv_buf = new char[out_iconv_buf_size];

	seg_buf_size = config->segment_buf_size;
	seg_buf = new char[seg_buf_size];

	return true;
}

Segment::~Segment()
{
	delete in_iconv_buf;
	delete out_iconv_buf;
	delete seg_buf;
	iconv_close(in_iconv);
	iconv_close(out_iconv);
}

} /* namespace ics */
} /* namespace jebe */
