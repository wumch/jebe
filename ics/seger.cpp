
#include "seger.hpp"
#include <glog/logging.h>
#include "config.hpp"

namespace jebe {
namespace ics {

Seger::Seger()
	: in_iconv_buf(NULL)
{
	if (prepare() != true)
	{
		LOG(ERROR) << "kid, prepare <Seger> failed!" << std::endl;
	}
}

void Seger::conv_in(char* content, size_t content_len)
{
	in_iconv_buf_cur = in_iconv_buf;
	in_iconv_buf_pos = in_iconv_buf_size;
	conv(in_iconv, &content, &content_len, &in_iconv_buf_cur, &in_iconv_buf_pos);
}

bool Seger::prepare()
{
	const Config* const config = Config::getInstance();

	icter.m_nOperateType = 2;
	icter.m_nOutputFormat = 2;

	internal_charset = config->internal_charset.c_str();
	external_charset = config->external_charset.c_str();

	in_iconv = iconv_open(internal_charset, external_charset);
	out_iconv = iconv_open(external_charset, internal_charset);
	CS_RETURN_IF(in_iconv == NULL || out_iconv == NULL, false);

	in_iconv_buf_pos = in_iconv_buf_size = config->iconv_buf_size;
	in_iconv_buf_cur = in_iconv_buf = new char[in_iconv_buf_size];

	return true;
}

Seger::~Seger()
{
	iconv_close(in_iconv);
	iconv_close(out_iconv);
	delete in_iconv_buf;
}

WordPOS WordPOSCal::wpos[wpos_end_val];
WordPOS WordPOSCal::empty_wpos;

void WordPOSCal::init()
{
	int nHandleSet[46]={24832,24932,24935,24942,25088,25344,25600,25703,25856,26112,26368,26624,26880,27136,27392,27648,27904,28160,28263,28274,28275,28276,28280,28282,28416,28672,28928,29184,29440,29696,29799,29952,30052,30055,30058,30060,30070,30074,30208,30308,30311,30318,30464,30720,30976,31232};						//   "a", "ad","ag","an","b", "c", "d", "dg","e", "f","g", "h", "i", "j", "k", "l", "m", "n", "ng","nr","ns","nt","nx","nz","o", "p", "q", "r", "s", "t", "tg","u", "ud","ug","uj","ul","uv","uz","v", "vd","vg","vn","w", "x", "y", "z"
	char sPOSRelated[46][3]={"a", "ad","ga","an","f", "c", "d", "d", "e","nd","g", "h", "i", "j", "k", "l", "m", "n", "gn","nh","ns","ni","ws", "nz","o", "p", "q", "r", "nl","nt","gt","u", "ud","ug","uj","ul","uv","uz","v", "vd","gv","vn","w", "x", "u", "a"};
	for (int i = 0; i < 46; ++i)
	{
		wpos[nHandleSet[i]] = WordPOS(sPOSRelated[i][0], sPOSRelated[i][1]);
	}
}

} /* namespace ics */
} /* namespace jebe */
