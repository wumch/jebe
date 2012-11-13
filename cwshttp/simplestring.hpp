
#pragma once

namespace jebe {
namespace cwshttp {

template<typename char_type_,typename size_type_>
class SimpleString
{
public:
	typedef char_type_ char_type;
	typedef size_type_ size_type;

protected:
	size_type size_;
	char_type* str;

public:
	size_type size() const { return size_; }

	SimpleString(char* str_);
};

}
}
