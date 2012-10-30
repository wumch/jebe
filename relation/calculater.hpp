
#pragma once

#include "predef.hpp"
#include <vector>
#include <list>
#include <boost/pool/pool_alloc.hpp>
#include <boost/unordered_map.hpp>
#include <glog/logging.h>
#include "math.hpp"
#include "config.hpp"
#include "aside.hpp"
#include "widcount.hpp"
#include "countedword.hpp"
#include "document.hpp"

namespace jebe {
namespace rel {

typedef boost::pool_allocator<DIdCount, boost::default_user_allocator_malloc_free,
	boost::details::pool::null_mutex, 1 << 20> DListAllocType;
typedef std::list<DIdCount, DListAllocType> DocCountList;

typedef DIdCount Proper;
typedef DocCountList ProperList;

class Calculater;

class VaredProperList
{
	friend class Calculater;
	static ProperList empty_plist;
protected:
	ProperList &plist;
	decimal_t ex;
	decimal_t var_sqrt;

protected:
	decimal_t ex_c() const
	{
		decimal_t psum = .0;
		for (ProperList::const_iterator it = plist.begin(); it != plist.end(); ++it)
		{
			psum += it->count;
			LOG_IF(INFO, Aside::config->loglevel > 2) << "it->count:" << it->count;
		}
		LOG_IF(INFO, Aside::config->loglevel > 2) << "plist.size():" << plist.size() << ", psum:" << psum << ", ex:" << (psum / Aside::totalDocNum);
		return psum / Aside::totalDocNum;
	}

	decimal_t var_sqrt_c() const
	{
		decimal_t d = ex * ex * (Aside::totalDocNum - plist.size());
		for (ProperList::const_iterator it = plist.begin(); it != plist.end(); ++it)
		{
			d += staging::square<decimal_t>(it->count - ex);
		}
		return std::sqrt(d / Aside::totalDocNum);
	}

public:
	VaredProperList()
		: plist(empty_plist), ex(.0), var_sqrt(.0)
	{}

	explicit VaredProperList(ProperList& plist_)
		: plist(plist_), ex(ex_c()), var_sqrt(var_sqrt_c())
	{}

	// to know how often being duplicated.
	VaredProperList(const VaredProperList& other)
		: plist(other.plist), ex(other.ex), var_sqrt(other.var_sqrt)
	{}

	const VaredProperList& operator=(const VaredProperList& other)
	{
		plist = other.plist;
		ex = other.ex;
		var_sqrt = other.var_sqrt;
		return *this;
	}

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
		var_sqrt = var_sqrt_c();
	}

	bool docEqual(docid_t docid, const Proper& proper) const
	{
		return docid == proper.id;
	}
};

class Calculater
{
private:
	static DListAllocType dlistAllocator;

protected:
	typedef boost::pool_allocator<DocCountList, boost::default_user_allocator_new_delete,
		boost::details::pool::null_mutex, 1 << _JEBE_WORD_MAP_HASH_BITS> WDListAllocType;
	typedef std::vector<DocCountList, WDListAllocType> WDList;	// word-doc map, actually a spare matrix.

	WDList wdlist;

	docnum_t mindf;
	docnum_t maxdf;

	typedef boost::fast_pool_allocator<boost::unordered_map<wordid_t, VaredProperList>::value_type, boost::default_user_allocator_new_delete,
			boost::details::pool::null_mutex, 1 << (_JEBE_WORD_MAP_HASH_BITS - 3)> ProperMapAllocType;
	typedef boost::unordered_map<wordid_t, VaredProperList, staging::BitsHash<(_JEBE_WORD_MAP_HASH_BITS - 3)>, std::equal_to<wordid_t>, ProperMapAllocType> WordProperMap;
	WordProperMap wpmap;

	typedef std::pair<wordid_t, decimal_t> Similarity;
	typedef std::list<Similarity> SimList;
	typedef boost::fast_pool_allocator<boost::unordered_map<wordid_t, SimList>::value_type, boost::default_user_allocator_new_delete,
			boost::details::pool::null_mutex, 1 << (_JEBE_WORD_MAP_HASH_BITS - 3)> SimListAllocType;
	typedef boost::unordered_map<wordid_t, SimList, staging::BitsHash<(_JEBE_WORD_MAP_HASH_BITS - 3)>, std::equal_to<wordid_t>, SimListAllocType> WordSimList;

	WordSimList wslist;

public:
	Calculater();

	void attachDoc(const Document& doc);

	void prepare();

	void calculate();

protected:
	void attachWord(wordid_t wordid, docid_t docid, wnum_t wordnum);

	void ready();

	void check();

	void filter();

	void calcu();

	void dump();

private:
	void filterByVarRate();

	CS_FORCE_INLINE size_t sum(const DocCountList& dlist) const;

	CS_FORCE_INLINE decimal_t corr(const VaredProperList& plist_1, const VaredProperList& plist_2) const;

	CS_FORCE_INLINE decimal_t cov(const VaredProperList& plist_1, const VaredProperList& plist_2) const;

	CS_FORCE_INLINE bool shouldSkip(const DocCountList& dlist) const;

	CS_FORCE_INLINE bool needFilterByVar() const;

	CS_FORCE_INLINE bool shouldSkipByVar(const VaredProperList& plist) const;
	CS_FORCE_INLINE bool shouldSkipByVar(decimal_t var) const;

	CS_FORCE_INLINE decimal_t getFilterVar(const VaredProperList& plist) const;
};

} /* namespace rel */
} /* namespace jebe */
