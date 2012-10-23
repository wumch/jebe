
#pragma once

#include "predef.hpp"
#include <boost/iterator/iterator_facade.hpp>
#include "autoincr.hpp"
#include "document.hpp"

namespace jebe {
namespace rel {

class BaseInput
{
private:
	class DocIdGenTag {};
	typedef staging::AutoIncr<DocIdGenTag> DocIdGen;

	DocIdGen *docIdGen;

protected:
	Document* const cur;
	docnum_t total_;

	void resetCurDoc(const char* buf, size_t len)
	{
		new (cur) Document(docIdGen->gen(), buf, len);
	}

	// just unpack received message.
	virtual void handleInput() = 0;

public:
	BaseInput()
		: docIdGen(DocIdGen::instance()),
		  cur(reinterpret_cast<Document*>(new char[sizeof(Document)])),
		  total_(0)
	{
	}

	virtual ~BaseInput()
	{
		delete docIdGen;
	}

	virtual void prepare() {}
	virtual void start() = 0;
	virtual void stop() {}
	virtual Document* next() = 0;

	virtual docnum_t total() const = 0;

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

} /* namespace rel */
} /* namespace jebe */
