
#pragma once

#include <string>
#include <list>

namespace jebe {
namespace cws {

class StrSeg
{
public:
	StrSeg() {}

	void segment(const std::string& str, std::list& res) const {}

	virtual ~StrSeg() {}
};

}
}
