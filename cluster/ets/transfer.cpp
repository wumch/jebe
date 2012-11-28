
#include "transfer.hpp"
#include "marve_holder.hpp"
#include "filter.hpp"

namespace jebe {
namespace cluster {
namespace ets {

void Transfer::trans(const InDocument& indoc, FeatureList& flist)
{
	holder.reset();
	flist.clear();
	Aside::filter->find(reinterpret_cast<const byte_t*>(indoc.text), indoc.text_size, holder);
	holder.genRes(flist);
}

} /* namespace ets */
} /* namespace cluster */
} /* namespace jebe */
