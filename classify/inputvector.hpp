
#pragma once

#include "predef.hpp"
#include <vector>
#include <iostream>
#include <msgpack.hpp>

namespace jebe {
namespace classify {

// for msgpack/unpck.
class Feature
{
public:
	fid_t fid;
	fval_t fval;

public:
	Feature() {}

	explicit Feature(fid_t fid_, fval_t fval_)
		: fid(fid_), fval(fval_)
	{}

	MSGPACK_DEFINE(fid, fval);
};

typedef std::vector<Feature> FeatureList;

class InputVector
{
protected:
	static size_t moffset;
	static msgpack::object mobj;
	static msgpack::zone mzone;

public:
	FeatureList flist;

public:
	InputVector()
	{}

	// consturct a document from a (usually received via socket) binarry buffer.
	// just to avoid from memory-copy.
	explicit InputVector(const char* buf, size_t len)
	{
		moffset = 0;
		if (CS_BLIKELY(msgpack::unpack(buf, len, &moffset, &mzone, &mobj) == msgpack::UNPACK_SUCCESS))
		{
			mobj.convert(this);
		}
		else
		{
			CS_DIE("kid, unpack <Document> from " << len << " bytes failed!");
		}
	}

	virtual ~InputVector()
	{
		mzone.~zone();
	}

	CS_FORCE_INLINE FeatureList* operator->()
	{
		return &flist;
	}
	CS_FORCE_INLINE const FeatureList* operator->() const
	{
		return &flist;
	}

	CS_FORCE_INLINE FeatureList& operator*()
	{
		return flist;
	}
	CS_FORCE_INLINE const FeatureList& operator*() const
	{
		return flist;
	}

	CS_FORCE_INLINE operator FeatureList() const
	{
		return flist;
	}

	MSGPACK_DEFINE(flist);
};

class InputSample: public InputVector
{
public:
	clsid_t belong_cls;

public:
	// consturct a document from a (usually received via socket) binarry buffer.
	// just to avoid from memory-copy.
	explicit InputSample(const char* buf, size_t len)
	{
		moffset = 0;
		if (CS_BLIKELY(msgpack::unpack(buf, len, &moffset, &mzone, &mobj) == msgpack::UNPACK_SUCCESS))
		{
			mobj.convert(this);
		}
		else
		{
			CS_DIE("kid, unpack <Document> from " << len << " bytes failed!");
		}
	}

	MSGPACK_DEFINE(belong_cls, flist);
};

} /* namespace classify */
} /* namespace jebe */
