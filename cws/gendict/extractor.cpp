/*
 * extractor.cpp
 *
 *  Created on: 2012-7-10
 *      Author: root
 */

#include "extractor.hpp"

namespace jebe {
namespace cws {

void Extractor::scan(const CString str, String::size_type len)
{
	String::size_type i = 0, chkPoint = 0;
	bool hasChs = false;
	while (i < len)
	{
		if (CS_BLIKELY(isGb2312(str[i])))
		{
			if (!hasChs)
			{
				hasChs = true;
			}
		}
		else
		{
			if (hasChs)
			{
				scanSentence(str + chkPoint, i - chkPoint, ph2map);
				scanSentence(str + chkPoint, i - chkPoint, ph3map);
				scanSentence(str + chkPoint, i - chkPoint, ph4map);
				scanSentence(str + chkPoint, i - chkPoint, ph5map);

				hasChs = false;
			}
			chkPoint = i + 1;
		}
		++i;
	}
}

template<uint8_t plen>
void Extractor::scanSentence(const CString str, String::size_type len,
		boost::unordered_map<Phrase<plen>, atimes_t>& phmap)
{
	typedef Phrase<plen> Ph;
	for (String::size_type i = 0, end = len - plen; i < end; ++i)
	{
		Ph ph(str + i, plen);
		if (CS_BUNLIKELY(phmap.find(ph) == phmap.end()))
		{
			phmap[ph] = 1;
		}
		else
		{
			++phmap[ph];
		}
	}
}

Extractor::Extractor() {
	// TODO Auto-generated constructor stub

}

Extractor::~Extractor() {
	// TODO Auto-generated destructor stub
}

} /* namespace cws */
} /* namespace jebe */
