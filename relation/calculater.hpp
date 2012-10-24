
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

	docnum_t maxdf;

public:
	Calculater();

	void attachDoc(const Document& doc);

	void prepare();

protected:
	void attachWord(wordid_t wordid, docid_t docid, wnum_t wordnum);

	void calculate();
};

} /* namespace rel */
} /* namespace jebe */
