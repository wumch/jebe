
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
weight_t WordPOSCal::weights[USHRT_MAX];

void WordPOSCal::init()
{
	int nHandleSet[46]={24832,24932,24935,24942,25088,25344,25600,25703,25856,26112,26368,26624,26880,27136,27392,27648,27904,28160,28263,28274,28275,28276,28280,28282,28416,28672,28928,29184,29440,29696,29799,29952,30052,30055,30058,30060,30070,30074,30208,30308,30311,30318,30464,30720,30976,31232};						//   "a", "ad","ag","an","b", "c", "d", "dg","e", "f","g", "h", "i", "j", "k", "l", "m", "n", "ng","nr","ns","nt","nx","nz","o", "p", "q", "r", "s", "t", "tg","u", "ud","ug","uj","ul","uv","uz","v", "vd","vg","vn","w", "x", "y", "z"
	char sPOSRelated[46][3]={"a", "ad","ga","an","f", "c", "d", "d", "e","nd","g", "h", "i", "j", "k", "l", "m", "n", "gn","nh","ns","ni","ws", "nz","o", "p", "q", "r", "nl","nt","gt","u", "ud","ug","uj","ul","uv","uz","v", "vd","gv","vn","w", "x", "u", "a"};
	for (int i = 0; i < 46; ++i)
	{
		wpos[nHandleSet[i]] = WordPOS(sPOSRelated[i][0], sPOSRelated[i][1]);
	}
	for (int i = 0; i < USHRT_MAX; ++i)
	{
		weights[i] = .0;
	}
	assignWeight('n', 3.0);				// 普通名词(n)
	assignWeight('n', 't', .0);			// 时间名词(nt)
	assignWeight('n', 'd', .0);			// 方位名词(nd)
	assignWeight('n', 'l', .0);			// 处所名词(nl)
	assignWeight('n', 'h', .0);			// 人名(nh)

	assignWeight('n', 's', 3.0);		//	地名(ns)
	assignWeight('n', 'n', 3.0);		//	族名(nn)
	assignWeight('n', 'i', 3.0);		//	团体机构名(ni)
	assignWeight('n', 'z', 3.0);		//	其他专有名词(nz)

	assignWeight('v', 2.0);				//	动词v：
	assignWeight('v', 'u', 1.0);		//	能愿动词(vu)
	assignWeight('v', 'd', 1.0);		//	趋向动词(vd)
	assignWeight('v', 'l', .0);			//	系动词(vl)

	assignWeight('a', 1.0);
	assignWeight('a', 'q', 1.0);		//	性质形容词(aq)
	assignWeight('a', 's', 1.0);		//	状态形容词(as)

	assignWeight('i', 1.0);				//	习用语(i)
	assignWeight('i', 'n', 1.0);		//	名词性习用语(in)
	assignWeight('i', 'v', 1.0);		//	动词性习用语(iv)
	assignWeight('i', 'a', 1.0);		//	形容词性习用语 (ia)
	assignWeight('i', 'c', 1.0);		//	连词性习用语(ic)

	assignWeight('j', 1.0);				//	简称和略语j
	assignWeight('j', 'n', 2.0);		//	名词性简称和略语 jn
	assignWeight('j', 'v', 2.0);		//	动词性简称和略语 jv
	assignWeight('j', 'a', 1.0);		//	形容词性简称和略语 ja
}

void WordPOSCal::assignWeight(uint8_t main, uint8_t sub, weight_t weight_)
{
	weights[(main << 8) + sub] = weight_;
}

void WordPOSCal::assignWeight(uint8_t main, weight_t weight_)
{
	weights[(main << 8)] = weight_;
}

} /* namespace ics */
} /* namespace jebe */
