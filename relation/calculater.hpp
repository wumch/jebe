
#pragma once

#include "predef.hpp"
#include <vector>
#include <boost/unordered_map.hpp>
#include "math.hpp"
#include "aside.hpp"
#include "widcount.hpp"
#include "countedword.hpp"
#include "document.hpp"

namespace jebe {
namespace rel {

class ProperTag {};
typedef IdCount<ProperTag, docid_t, decimal_t> Proper;
typedef std::vector<Proper> ProperList;

class Calculater;

class VaredProperList
{
	friend class Calculater;
protected:
	ProperList plist;
	decimal_t ex;
	decimal_t ex_square;
	decimal_t var_sqrt;

protected:
	decimal_t ex_c() const
	{
		decimal_t psum = .0;
		for (ProperList::const_iterator it = plist.begin(); it != plist.end(); ++it)
		{
			psum += it->count;
		}
		return psum / Aside::totalDocNum;
	}

	decimal_t var_sqrt_c() const
	{
		decimal_t d = ex_square * (Aside::totalDocNum - plist.size());
		for (ProperList::const_iterator it = plist.begin(); it != plist.end(); ++it)
		{
			d += staging::square(it->count - ex);
		}
		return std::sqrt(d);
	}

public:
	VaredProperList()
		: ex(.0), ex_square(.0), var_sqrt(.0)
	{}

	explicit VaredProperList(const ProperList& plist_)
		: plist(plist_), ex(ex_c()), ex_square(ex * ex), var_sqrt(var_sqrt_c())
	{}

	ProperList* operator->()
	{
		return &plist;
	}

	const ProperList* operator->() const
	{
		return &plist;
	}

	void reCalculate()
	{
		ex = ex_c();
		ex_square = ex * ex;
		var_sqrt = var_sqrt_c();
	}

	bool docEqual(docid_t docid, const Proper& proper) const
	{
		return docid == proper.id;
	}

	decimal_t properOnDoc(docid_t docid) const;
};

class Calculater
{
protected:
	typedef std::vector<DIdCount> DocCountList;
	typedef std::vector<DocCountList> WDList;	// word-doc map, actually a spare matrix.

	WDList wdlist;

	docnum_t mindf;
	docnum_t maxdf;

	typedef boost::unordered_map<wordid_t, VaredProperList> WordProperMap;
	WordProperMap wpmap;

	typedef std::pair<wordid_t, decimal_t> Similarity;
	typedef std::list<Similarity> SimList;
	typedef boost::unordered_map<wordid_t, SimList> WordSimList;

	WordSimList wslist;

public:
	Calculater();

	void attachDoc(const Document& doc);

	void prepare();

	void calculate();

protected:
	void attachWord(wordid_t wordid, docid_t docid, wnum_t wordnum);

	void randomAttachWord(wordid_t wordid, docid_t docid, wnum_t atimes);

	void ready();

	void check();

	void filter();

	void calcu();

	void dump();

private:
	inline size_t sum(const DocCountList& dlist) const;

	void toProper(const DocCountList& dlist, VaredProperList& plist) const;

	decimal_t cov(const VaredProperList& plist_1, const VaredProperList& plist_2) const;

	bool shouldSkip(const DocCountList& dlist) const;
};

} /* namespace rel */
} /* namespace jebe */
