
#pragma once

#include "../../staging/staging.hpp"
#include <string>
#include <vector>
#include <bitset>
#include <string.h>
#include <boost/unordered_map.hpp>
#include "../../staging/hash.hpp"
#include "../../staging/array.hpp"

namespace jebe {
namespace cws {

typedef uint32_t atimes_t;
typedef wchar_t	CharType;
typedef CharType* CString;
typedef std::wstring String;

template<uint8_t length>
class Phrase
{
public:
	static const uint8_t len = length;
	typedef wchar_t StrType[len];

protected:
	StrType str;

public:
	Phrase(const CharType* str_, uint8_t length_)
	{
		strncpy(str_, str, length_);
	}

	// half-hash
	uint32_t hfhash() const
	{
		typedef uint32_t mask_t;
		if (sizeof(CharType) == sizeof(mask_t))
		{
			switch (length)
			{
			case 2:
				return str[0] ^ str[1];
			case 3:
				return str[0] ^ str[1] ^ str[2];
			case 4:
				return str[0] ^ str[1] ^ str[2] ^ str[3];
			case 5:
				return str[0] ^ str[1] ^ str[2] ^ str[3] ^ str[4];
			default:
				mask_t mask = 0;
				for (uint8_t i = 0; i < length; ++i)
				{
					mask ^= str[i];
				}
				return mask;
			}
		}
		else
		{
			mask_t mask = 0;
			char* res = reinterpret_cast<char*>(&mask);
			char* str_ = reinterpret_cast<char*>(str);
			for (uint8_t i = 0, end = length * sizeof(CharType); i < end; ++i)
			{
				res[i & 3] ^= str_[i];
			}
			return mask;
		}
	}
};


//typedef std::wstring String;
typedef Phrase<2> Ph2;
typedef Phrase<3> Ph3;
typedef Phrase<4> Ph4;
typedef Phrase<5> Ph5;
typedef boost::unordered_map<Ph2, atimes_t> Ph2Map;
typedef boost::unordered_map<Ph3, atimes_t> Ph3Map;
typedef boost::unordered_map<Ph4, atimes_t> Ph4Map;
typedef boost::unordered_map<Ph5, atimes_t> Ph5Map;

class Extractor
{
protected:
	static const int32_t gb_char_max = 65536;
	std::bitset<gb_char_max> gb2312;

	Ph2Map ph2map;
	Ph3Map ph3map;
	Ph4Map ph4map;
	Ph5Map ph5map;

public:
	void extract(const String& str);
	void scan(const CString str, String::size_type len);

	template<uint8_t plen>
	void scanSentence(const CString str, String::size_type len,
			boost::unordered_map<Phrase<plen>, atimes_t>& phmap);

	bool isGb2312(CharType c) const
	{
		return c < gb_char_max && gb2312[c];
	}

	Extractor();
	virtual ~Extractor();
};

} /* namespace cws */
} /* namespace jebe */
