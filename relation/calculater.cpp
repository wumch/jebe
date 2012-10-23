
#include "calculater.hpp"
#include "aside.hpp"
#include "document.hpp"

namespace jebe {
namespace rel {

Calculater::Calculater()
{

}

void Calculater::attachDoc(const Document& doc)
{
	for (Document::WordList::const_iterator it = doc.words.begin(); it != doc.words.end(); ++it)
	{
		attachWord(doc.id, Aside::wordmap.getWordId(it->word), it->count);
	}
	++Aside::curDocNum;
}

void Calculater::attachWord(docid_t docid, wordid_t wordid, wnum_t wordnum)
{
//	std::binary_search(wdmap[wordid].begin(), wdmap[wordid].end(), std::equal_to<DIdCount>());
}

} /* namespace rel */
} /* namespace jebe */
