
#pragma once

#include "predef.hpp"
#include <vector>
#include "widcount.hpp"
#include "countedword.hpp"
#include "document.hpp"

namespace jebe {
namespace rel {

class Calculater
{
protected:
	typedef std::vector<DIdCount> DocCountList;
	typedef std::vector<DocCountList> WDMap;	// word-doc map, actually a spare matrix.

	WDMap wdmap;

public:
	Calculater();

	void attachDoc(const Document& doc);

protected:
	void attachWord(docid_t docid, wordid_t wordid, wnum_t wordnum);
};

} /* namespace rel */
} /* namespace jebe */
