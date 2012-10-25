
#include "calculater.hpp"
#include <exception>
#include <fstream>
#include <iostream>
#include <clocale>
#include <boost/lexical_cast.hpp>
#include "aside.hpp"
#include "config.hpp"
#include "document.hpp"

namespace jebe {
namespace rel {

class DocDuplicated
	: public std::exception
{
private:
	wordid_t wordid;
	docid_t docid;
	mutable std::string reason;

public:
	DocDuplicated(wordid_t wordid_, docid_t docid_)
		: wordid(wordid_), docid(docid_)
	{}

	virtual const char* what() const throw()
	{
		if (reason.empty())
		{
			reason += "word(" + boost::lexical_cast<std::string>(wordid) + ") & document(" + boost::lexical_cast<std::string>(docid) + ") duplicated";
		}
		return reason.c_str();
	}

	virtual ~DocDuplicated() throw() {}
};

Calculater::Calculater()
{

}

void Calculater::attachDoc(const Document& doc)
{
	for (Document::WordList::const_iterator it = doc.words.begin(); it != doc.words.end(); ++it)
	{
		attachWord(Aside::wordmap.getWordId(it->word), doc.id, it->count);
	}
	++Aside::curDocNum;
}

void Calculater::attachWord(wordid_t wordid, docid_t docid, wnum_t atimes)
{
	wdlist[wordid].push_back(DIdCount(docid, atimes));	// newer document is guaranteed to be greater than older.
}

void Calculater::prepare()
{
	CS_SAY(wdlist.size());
	wdlist.assign(Aside::wordmap.map.size(), DocCountList());
	CS_SAY(wdlist.size());
}

void Calculater::calculate()
{
	ready();
	check();
	filter();
	calcu();
	dump();
}

void Calculater::ready()
{
	std::vector<wnum_t> worddf;
	worddf.reserve(wdlist.size());
	for (wordid_t wordid = 0; wordid < wdlist.size(); ++wordid)
	{
		if (!wdlist[wordid].empty())
		{
			worddf.push_back(wdlist[wordid].size());
		}
	}
	std::sort(worddf.begin(), worddf.end());
	maxdf = worddf[static_cast<wordid_t>(Aside::config->df_quantile_top * worddf.size())];
	mindf = worddf[static_cast<wordid_t>(Aside::config->df_quantile_bottom * worddf.size())];
	CS_DUMP(worddf.size());
	CS_DUMP(maxdf);
	CS_DUMP(mindf);
}

void Calculater::filter()
{
	for (wordid_t wordid = 0; wordid < wdlist.size(); ++wordid)
	{
		if (!shouldSkip(wdlist[wordid]))
		{
			toProper(wdlist[wordid], wpmap.insert(std::make_pair(wordid, VaredProperList())).first->second);
		}
		wdlist[wordid].clear();
	}
	wdlist.clear();
}

void Calculater::calcu()
{
	decimal_t c = .0;
	for (WordProperMap::const_iterator it = wpmap.begin(); it != wpmap.end(); ++it)
	{
		const VaredProperList& plist_1 = it->second;
		SimList& simlist = wslist.insert(std::make_pair(it->first, SimList())).first->second;
		for (WordProperMap::const_iterator iter = wpmap.begin(); iter != wpmap.end(); ++iter)
		{
			if (CS_BLIKELY(iter != it))
			{
				c = cov(plist_1, iter->second);
				CS_SAY("cov(" << Aside::wordmap.getWordById(it->first) << "," << Aside::wordmap.getWordById(iter->first) << ") = " << c);
				if (c >= Aside::config->min_corr)
				{
					simlist.push_back(Similarity(iter->first, c));
				}
			}
		}
	}
}

decimal_t Calculater::cov(const VaredProperList& plist_1, const VaredProperList& plist_2) const
{
	decimal_t exy = .0;
	for (ProperList::const_iterator it = plist_1->begin(); it != plist_1->end(); ++it)
	{
		exy += plist_2.properOnDoc(it->id);
	}
	return exy - plist_1.ex * plist_2.ex;
}

size_t Calculater::sum(const DocCountList& dlist) const
{
	size_t res = 0;
	for (DocCountList::const_iterator it = dlist.begin(); it != dlist.end(); ++it)
	{
		res += it->count;
	}
	return res;
}

void Calculater::toProper(const DocCountList& dlist, VaredProperList& plist) const
{
	size_t atimes = sum(dlist);
	for (docnum_t i = 0; i < dlist.size(); ++i)
	{
		plist->push_back(Proper(dlist[i].id, static_cast<decimal_t>(dlist[i].count) / atimes));
	}
	plist.reCalculate();
}

bool Calculater::shouldSkip(const DocCountList& dlist) const
{
	return dlist.empty() || dlist.size() < mindf || maxdf < dlist.size();
}

void Calculater::check()
{
	for (wordid_t wid = 0; wid < wdlist.size(); ++wid)
	{
		const DocCountList& dlist = wdlist[wid];
#if CS_DEBUG
		if (!dlist.empty())
		{
			CS_STDOUT << "word:[" << Aside::wordmap[wid] << "] => (";
		}
#endif
		for (docnum_t i = 1; i < dlist.size(); ++i)
		{
#if CS_DEBUG
			if (!dlist.empty())
			{
				CS_STDOUT << i << ',';
			}
#endif
			if (CS_BUNLIKELY(!(dlist[i - 1] < dlist[i])))
			{
				CS_DIE("kid, the order of [{document-id,word-appear-times}...][" << i << "] of some word is wrong!");
			}
		}
#if CS_DEBUG
		if (!dlist.empty())
		{
			CS_STDOUT << ")" << std::endl;
		}
#endif
	}
	CS_SAY("all of order are correct!");
}

void Calculater::dump()
{
	std::ofstream ofile(Aside::config->outputfile.string().c_str(), std::ios_base::trunc);
	ofile.imbue(std::locale(""));
	for (WordSimList::const_iterator it = wslist.begin(); it != wslist.end(); ++it)
	{
		const SimList& simlist = it->second;
		for (SimList::const_iterator iter = simlist.begin(); iter != simlist.end(); ++iter)
		{
			ofile << Aside::wordmap[it->first] << '\t' << Aside::wordmap[iter->first] << '\t' << iter->second << CS_LINESEP;
			CS_SAY("in word-similarity-list, cov(" << Aside::wordmap.getWordById(it->first) << "," << Aside::wordmap.getWordById(iter->first) << ") = " << iter->second);
		}
	}
	ofile.close();
}

void Calculater::randomAttachWord(wordid_t wordid, docid_t docid, wnum_t atimes)
{
	DocCountList& dlist = wdlist[wordid];
	DIdCount dic(docid, atimes);
	if (CS_BUNLIKELY(dlist.empty()))
	{
		dlist.push_back(dic);
	}
	else
	{
		docnum_t left = 0, right = dlist.size() - 1;
		docnum_t cur = (left + right) >> 1;
		while (left < cur && cur < right)
		{
			if (CS_BUNLIKELY(dlist[cur].id == docid))
			{
				throw DocDuplicated(wordid, docid);
			}
			if (dlist[cur].id < docid)
			{
				left = cur;
			}
			else
			{
				right = cur;
			}
			cur = (left + right) >> 1;
		}
		if (CS_BLIKELY(dlist[right] < dic))
		{
			dlist.insert(dlist.end(), dic);
		}
		else
		{
			if (dic < dlist[left])
			{
				cur = left;
			}
			else if (CS_BUNLIKELY(dic == dlist[left]))
			{
				throw DocDuplicated(wordid, docid);
			}
			else if (dic < dlist[right])
			{
				cur = right;
			}
			else if (CS_BUNLIKELY(dic == dlist[right]))
			{
				throw DocDuplicated(wordid, docid);
			}
			dlist.insert(DocCountList::iterator(&dlist[cur]), dic);
		}
	}
}

decimal_t VaredProperList::properOnDoc(docid_t docid) const
{
	if (CS_BUNLIKELY(plist.empty()))
	{
		return .0;
	}
	docnum_t left = 0, right = plist.size() - 1;
	docnum_t cur = (left + right) >> 1;
	while (left < cur && cur < right)
	{
		if (plist[cur].id == docid)
		{
			return plist[cur].count;
		}
		else if (plist[cur].id < docid)
		{
			left = cur;
		}
		else
		{
			right = cur;
		}
		cur = (left + right) >> 1;
	}
	return .0;
}

} /* namespace rel */
} /* namespace jebe */