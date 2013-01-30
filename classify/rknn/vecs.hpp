
#pragma once

#include "predef.hpp"
#include <vector>
#include <boost/iterator/iterator_facade.hpp>
#include "vector.hpp"
#include "aside.hpp"

namespace jebe {
namespace classify {
namespace rknn {

class VecFactory
{
public:
	static Vector* create(const InputVector& doc)
	{
		return new Vector(doc);
	}
};

class Clses;
class SampleGroups;

class Cls
{
	friend class Clses;
	friend class SampleGroups;
	friend class Calculator;
private:
	typedef std::vector<const Sample*> MemberList;
	MemberList members;

	clsid_t id;
	Vector ropt;	// Rocchio optimized matcher.

public:
	Cls();

	void attach(const Sample* s)
	{
		members.push_back(s);
	}

	MemberList* operator->()
	{
		return &members;
	}

	const MemberList* operator->() const
	{
		return &members;
	}

	MemberList& operator*()
	{
		return members;
	}
	const MemberList& operator*() const
	{
		return members;
	}
};

class Calculator;

// all of <Cls>s.
class Clses
{
	friend class SampleGroups;
	friend class Calculator;
private:
	typedef std::vector<Cls> ClsList;
	ClsList clses;

	clsnum_t cls_num;

public:
	void deliver(const SamplePool& samples);

	const ClsList* operator->() const
	{
		return &clses;
	}

	const ClsList& operator*() const
	{
		return clses;
	}
};

template<typename ContainerType>
class SampleIterator
	: public boost::iterator_facade<SampleIterator<ContainerType>, const Sample, boost::forward_traversal_tag>
{
private:
	typedef ContainerType Container;
	typedef SampleIterator<Container> iterator;

public:
	explicit SampleIterator(Container& container_)
		: container(container_), m_sample(NULL)
	{}

	void increment()
	{
		m_sample = container.next();
	}

	bool equal(const iterator& other) const
	{
		return m_sample == other.m_sample;
	}

	const Sample& dereference() const
	{
		return *m_sample;
	}

	void locate(const Sample* s)
	{
		m_sample = s;
	}

	const Sample* operator->() const
	{
		return m_sample;
	}

private:
	Container& container;

	const Sample* m_sample;
};

// iterate <Sample> over <Cls>s.
class SampleGroups
{
public:
	typedef SampleIterator<SampleGroups> iterator;
	typedef iterator const_iterator;		// must be same, be not reference.

private:
	const Clses& clses;

	ClsIdList cls_ids;
	clsnum_t cur_cls_idx;
	vnum_t cur_vec_idx;
	bool reached_end;

	const iterator last_;
	const iterator first_;

	mutable iterator iter;

private:
	clsid_t cur_cls_id() const
	{
		return cls_ids[cur_cls_idx];
	}

	iterator wrap(const Sample* s) const
	{
		iter.locate(s);
		return iter;
	}

public:
	SampleGroups(const Clses& clses_, const ClsIdList& cls_ids_)
		: clses(clses_), cls_ids(cls_ids_),
		  cur_cls_idx(0), cur_vec_idx(0), reached_end(false),
		  last_(*this), first_(wrap(empty() ? NULL : (*(*clses)[0])[0])), iter(begin())
	{}

	const Sample* next()
	{
		if (reached_end)
		{
			return NULL;
		}
		if (!(++cur_vec_idx < (*clses)[cur_cls_id()]->size()))
		{
			if (++cur_cls_idx < cls_ids.size())
			{
				cur_vec_idx = 0;
			}
			else
			{
				reached_end = true;
				return NULL;
			}
		}
		return (*(*clses)[cur_cls_id()])[cur_vec_idx];
	}

	bool empty() const
	{
		return clses->empty() || (*clses)[0]->empty();
	}

	iterator begin() const
	{
		return first_;
	}

	iterator end() const
	{
		return last_;
	}
};

} /* namespace rknn */
} /* namespace classify */
} /* namespace jebe */
