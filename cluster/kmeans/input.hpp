
#pragma once

#include "predef.hpp"
#include <boost/iterator/iterator_facade.hpp>
#include <new>
#include "autoincr.hpp"
#include "../document.hpp"

namespace jebe {
namespace cluster {
namespace kmeans {

class BaseInput
{
protected:
	Document* const cur;

	void resetCurDoc(const char* buf, size_t len)
	{
		new (cur) Document(buf, len);
	}

public:
	BaseInput()
		: cur(reinterpret_cast<Document*>(new char[sizeof(Document)]))
	{
	}

	virtual ~BaseInput()
	{}

	virtual void prepare() {}
	virtual void start() = 0;

	virtual void stop()
	{
		Document::finalize();
	}

	virtual Document* next() = 0;

public:
	Document& curdoc()
	{
		return *cur;
	}
};

class DocIter
	: public boost::iterator_facade<DocIter, Document, boost::forward_traversal_tag>
{
	friend class boost::iterator_core_access;

private:
	BaseInput* const inputer;

public:
	explicit DocIter(BaseInput* inputer_)
		: inputer(inputer_)
	{
	}

	DocIter(const DocIter& iter)
		: inputer(iter.inputer)
	{
	}

	void increment()
	{
		inputer->next();
	}

	bool equal(const DocIter& other) const
	{
		return inputer == other.inputer;
	}

	Document& dereference() const
	{
		return inputer->curdoc();
	}
};

} /* namespace kmeans */
} /* namespace cluster */
} /* namespace jebe */
