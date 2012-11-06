
#include "calculater.hpp"
#include <exception>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <clocale>
#include <boost/lexical_cast.hpp>
#include <glog/logging.h>
#include "math.hpp"
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

ProperList VaredProperList::empty_plist;

Calculater::Calculater()
	: wpmap(1 << _JEBE_WORD_MAP_HASH_BITS), cur_sim_num(0), wslist(100)
{}

void Calculater::attachDoc(const Document& doc)
{
	for (Document::WordList::const_iterator it = doc.words.begin(); it != doc.words.end(); ++it)
	{
		attachWord(Aside::wordmap[it->word], doc.id, it->count);
	}
	++Aside::curDocNum;
}

void Calculater::attachWord(wordid_t wordid, docid_t docid, wnum_t atimes)
{
	wdlist[wordid].push_back(DIdCount(docid, CS_BLIKELY(atimes < 256) ? atimes : 255));	// newer document is guaranteed to be greater than older.
}

void Calculater::prepare()
{
	CS_DUMP(wdlist.size());
	wdlist.assign(Aside::wordmap.map.size(), DocCountList());
	CS_DUMP(wdlist.size());
}

void Calculater::calculate()
{
	ready();
#if CS_DEBUG
	check();
#endif
	filter();
	calcu();
	dump();
	finish();
}

void Calculater::ready()
{
	Aside::totalDocNum = Aside::curDocNum;
	std::vector<docnum_t> worddf;
	worddf.reserve(wdlist.size());
	for (wordid_t wordid = 0; wordid < wdlist.size(); ++wordid)
	{
		if (!wdlist[wordid].empty())
		{
			worddf.push_back(wdlist[wordid].size());
		}
	}
	std::sort(worddf.begin(), worddf.end());
	maxdf = worddf[static_cast<wordid_t>((1.0 - Aside::config->df_quantile_top) * (worddf.size() - 1))];
	mindf = std::max(2u, worddf[static_cast<wordid_t>(Aside::config->df_quantile_bottom * (worddf.size() - 1))]);
	LOG_IF(INFO, Aside::config->loglevel > 0) << "statistics:" << CS_LINESEP <<
		"total-documents: " << Aside::totalDocNum << CS_LINESEP <<
		"doc-frequency: [" << Aside::config->df_quantile_bottom << ", " << (1 - Aside::config->df_quantile_top) << "] = [" << mindf << ", " << maxdf << "]" << CS_LINESEP <<
		"word-doc-var: [" << Aside::config->min_wd_var << ", " << Aside::config->max_wd_var << "]" << CS_LINESEP <<
		"word-doc-var-rate-range: [" << Aside::config->wd_var_bottom << ", " << (1.0 - Aside::config->wd_var_top) << "]" << CS_LINESEP <<
		"required corr: [" << Aside::config->min_word_corr << ", " << Aside::config->max_word_corr << "]" << CS_LINESEP;
}

void Calculater::calcu()
{
	for (WordProperMap::const_iterator it = wpmap.begin(); it != wpmap.end(); ++it)
	{
		const VaredProperList& plist_1 = it->second;
		SimList& simlist = wslist.insert(std::pair<wordid_t, SimList>(it->first, SimList())).first->second;
		WordProperMap::const_iterator iter = it;

		for (ProperList::const_iterator i = (*plist_1)->begin(); i != (*plist_1)->end(); ++i)
		{
			CS_PREFETCH(&*i, 0, 3);
		}

		while (++iter != wpmap.end())
		{
			recordCorr(iter->first, corr(plist_1, iter->second), simlist);
		}

		if (cur_sim_num >= Aside::config->sim_buff_size)
		{
			dump();
			wslist.clear();
			cur_sim_num = 0;
		}
	}
	dump();
	wslist.clear();
	cur_sim_num = 0;
	LOG_IF(INFO, Aside::config->loglevel > 0) << "calculated" << CS_LINESEP;
}

void Calculater::recordCorr(wordid_t wordid, decimal_t corr, SimList& simlist)
{
	if (Aside::config->min_word_corr <= corr && corr <= Aside::config->max_word_corr)
	{
		simlist.push_back(Similarity(wordid, corr));
		++cur_sim_num;
	}
}

decimal_t Calculater::corr(const VaredProperList& plist_1, const VaredProperList& plist_2) const
{
	return CS_BUNLIKELY(plist_1.var_sqrt == 0 || plist_2.var_sqrt == 0) ? .0 : (cov(plist_1, plist_2) / (plist_1.var_sqrt * plist_2.var_sqrt));
}

// cov(X,Y) = (X - Ex)*(Y - Ey)
decimal_t Calculater::cov(const VaredProperList& plist_1, const VaredProperList& plist_2) const
{
	decimal_t summary = 0;
	bool used = false;
	docnum_t k = 0;

	for (ProperList::const_iterator i = (*plist_1)->begin(), j = (*plist_2)->begin(); i != (*plist_1)->end(); ++i)
	{
		for (; j != (*plist_2)->end(); ++j)
		{
			if (j->id < i->id)
			{
				summary -= plist_1.ex * (j->count - plist_2.ex);
				++k;
			}
			else if (j->id == i->id)
			{
				summary += (i->count - plist_1.ex) * (j->count - plist_2.ex);
				++j;
				++k;
				used = true;
				break;
			}
			else
			{
				break;
			}
		}
		if (CS_BLIKELY(!used))
		{
			summary -= (i->count - plist_1.ex) * plist_2.ex;
			++k;
		}
		else
		{
			used = false;
		}
	}
	return (summary + (Aside::totalDocNum - k) * plist_1.ex * plist_2.ex) / Aside::totalDocNum;
}

size_t Calculater::sum(const DocCountList& dlist) const
{
	size_t res = 0;
	for (DocCountList::const_iterator it = dlist->begin(); it != dlist->end(); ++it)
	{
		res += it->count;
	}
	return res;
}

void Calculater::filter()
{
	LOG_IF(INFO, Aside::config->loglevel > 0) << "before filter, total words: " << wdlist.size() << CS_LINESEP;
	if (needFilterByVar())
	{
		for (wordid_t wordid = 0; wordid < wdlist.size(); ++wordid)
		{
			if (!shouldSkip(wdlist[wordid]))
			{
				VaredProperList plist(wdlist[wordid]);
				if (!shouldSkipByVar(plist))
				{
					wpmap.insert(std::pair<wordid_t, VaredProperList>(wordid, plist));
				}
				else
				{
					LOG_IF(INFO, Aside::config->loglevel > 2) << "[" << Aside::wordmap[wordid] << "] filtered by var = " << plist.var_sqrt << "/" << plist.ex;
					wdlist[wordid].resize(0);
				}
			}
			else
			{
				LOG_IF(INFO, Aside::config->loglevel > 2) << "[" << Aside::wordmap[wordid] << "] filtered by df = " << wdlist[wordid].size();
				wdlist[wordid].resize(0);
			}
		}
		LOG_IF(INFO, Aside::config->loglevel > 0) << "filtered by df and non-rate-var, remain words: " << wpmap.size() << CS_LINESEP;
	}
	else
	{
		for (wordid_t wordid = 0; wordid < wdlist.size(); ++wordid)
		{
			if (!shouldSkip(wdlist[wordid]))
			{
				wpmap.insert(std::pair<wordid_t, VaredProperList>(wordid, VaredProperList(wdlist[wordid])));
			}
			else
			{
				LOG_IF(INFO, Aside::config->loglevel > 2) << Aside::wordmap[wordid] << "filtered by df = " << wdlist[wordid].size();
				wdlist[wordid].resize(0);
			}
		}
		LOG_IF(INFO, Aside::config->loglevel > 0) << "filtered by df, remain words: " << wpmap.size() << CS_LINESEP;
	}
	filterByVarRate();
}

void Calculater::filterByVarRate()
{
	CS_RETURN_IF(!(Aside::config->wd_var_bottom > 0 || staging::between_open(Aside::config->wd_var_top, .0, 1.0)));
	CS_RETURN_IF(wpmap.empty());

	LOG_IF(INFO, Aside::config->loglevel > 0) << "starting filter by var-rate";

	std::vector<decimal_t> varlist;
	varlist.reserve(wpmap.size());
	for (WordProperMap::const_iterator it = wpmap.begin(); it != wpmap.end(); ++it)
	{
		varlist.push_back(getFilterVar(it->second));
	}
	std::sort(varlist.begin(), varlist.end());

	decimal_t min_var = varlist[static_cast<wordid_t>((varlist.size() - 1) * Aside::config->wd_var_bottom)];
	decimal_t max_var = varlist[static_cast<wordid_t>((varlist.size() - 1) * (1.0 - Aside::config->wd_var_top))];

	LOG_IF(INFO, Aside::config->loglevel > 0) << "will filter by var-rate not between [" << min_var << "," << max_var << "]";
	CS_RETURN_IF(!(min_var < max_var));

	for (WordProperMap::iterator it = wpmap.begin(); it != wpmap.end(); )
	{
		if (!staging::between(getFilterVar(it->second), min_var, max_var))
		{
			LOG_IF(INFO, Aside::config->loglevel > 2) << "[" << Aside::wordmap[it->first] << "] filtered by var-rate = " << it->second.var_sqrt << "/" << it->second.ex;
			wdlist[it->first].resize(0);
			it = wpmap.erase(it);
		}
		else
		{
			++it;
		}
	}
	LOG_IF(INFO, Aside::config->loglevel > 0) << "filtered by var-rate, remain words: " << wpmap.size();
}

decimal_t Calculater::getFilterVar(const VaredProperList& plist) const
{
	return plist.var_sqrt / plist.ex;
}

bool Calculater::shouldSkip(const DocCountList& dlist) const
{
	return dlist.empty() || dlist.size() < mindf || maxdf < dlist.size();
}

bool Calculater::needFilterByVar() const
{
	return Aside::config->min_wd_var > 0 || Aside::config->max_wd_var > 0;
}

bool Calculater::shouldSkipByVar(const VaredProperList& plist) const
{
	return shouldSkipByVar(plist.var_sqrt / plist.ex);
}

bool Calculater::shouldSkipByVar(decimal_t var) const
{
	return Aside::config->min_wd_var > var || var < Aside::config->max_wd_var;
}

void Calculater::check()
{
	for (wordid_t wid = 0; wid < wdlist.size(); ++wid)
	{
		const DocCountList& dlist = wdlist[wid];
		for (DocCountList::const_iterator i = dlist->begin(), prev = i++; i != dlist->end(); ++i)
		{
			if (CS_BUNLIKELY(!(prev->id < i->id)))
			{
				CS_DIE("kid, the order of [{document-id,word-appear-times}...][] of some word is wrong!");
			}
		}
	}
	LOG_IF(INFO, Aside::config->loglevel > 0) << "checked, all of order are correct!";
}

void Calculater::dump()
{
	static std::ofstream ofile(Aside::config->outputfile.string().c_str(), std::ios_base::trunc);
	static bool inited = false;
	if (!inited)
	{
		ofile.imbue(std::locale(""));
		inited = true;
	}
	for (WordSimList::const_iterator it = wslist.begin(); it != wslist.end(); ++it)
	{
		const SimList& simlist = it->second;
		for (SimList::const_iterator iter = simlist.begin(); iter != simlist.end(); ++iter)
		{
			ofile << Aside::wordmap[it->first] << '\t' << Aside::wordmap[iter->first] << '\t' << iter->second << CS_LINESEP;
		}
	}
	LOG_IF(INFO, Aside::config->loglevel > 0) << "dumped " << cur_sim_num << " to " << Aside::config->outputfile << CS_LINESEP;
}

void Calculater::finish()
{
//	ofile.close();
}

} /* namespace rel */
} /* namespace jebe */
