
#include "calculater.hpp"
#include <exception>
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
	DocCountList& dlist = wdmap[wordid];
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
		if (dic < dlist[right])
		{
			cur = right;
		}

		dlist.insert(DocCountList::iterator(&dlist[left]), dic);
	}
}

void Calculater::prepare()
{
	wdmap.assign(Aside::wordmap.map.size(), DocCountList());
	maxdf = (Aside::config->df_quantile_bottom + Aside::config->df_quantile_top) * Aside::totalDocNum;
}

} /* namespace rel */
} /* namespace jebe */
