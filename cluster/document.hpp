
#pragma once

#include "predef.hpp"
#include <string>
#include <boost/numeric/ublas/vector_sparse.hpp>
#include <vector>
#include <iostream>
#include <boost/noncopyable.hpp>
#include <msgpack.hpp>

namespace jebe {
namespace cluster {

typedef boost::numeric::ublas::map_array<fid_t, fval_t> FeatureArray;

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
	{
	}

	MSGPACK_DEFINE(fid, fval);
};

class Document
	: private boost::noncopyable
{
private:
	static size_t moffset;
	static msgpack::object mobj;
	static msgpack::zone mzone;

public:
	typedef std::vector<Feature> FeatureList;

	vid_t id;
	FeatureList flist;

public:
	explicit Document(vid_t id_, const FeatureList& words_)
		: id(id_), flist(words_)
	{}

	// consturct a document from a (usually received via socket) binarry buffer.
	// just to avoid from memory-copy.
	explicit Document(const char* buf, size_t len)
	{
		moffset = 0;
		if (CS_BLIKELY(msgpack::unpack(buf, len, &moffset, &mzone, &mobj) == msgpack::UNPACK_SUCCESS))
		{
			mobj.convert(this);
		}
		else
		{
			CS_DIE("kid, unpack <Document(" << id << ")> from " << len << " bytes failed!");
		}
	}

	explicit Document(vid_t id_, const FeatureArray& vec)
		: id(id_)
	{
		flist.reserve(vec.size());
		for (FeatureArray::const_iterator it = vec.begin(); it != vec.end(); ++it)
		{
			flist.push_back(Feature(it->first, it->second));
		}
	}

public:
	static void finalize()
	{
		mzone.~zone();
	}

	MSGPACK_DEFINE(id, flist);
};

static inline bool operator==(const Document& doc_1, const Document& doc_2)
{
	return &doc_1.id == &doc_2.id;
}

} /* namespace cluster */
} /* namespace jebe */
