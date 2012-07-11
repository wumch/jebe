
#pragma once

#include "staging.hpp"
#include <vector>
#include <algorithm>
#include <bitset>
#include <string.h>
#include <boost/unordered_map.hpp>
#include <boost/filesystem.hpp>
#include "hash.hpp"
#include "array.hpp"

namespace jebe {
namespace cws {

typedef uint32_t atimes_t;
typedef wchar_t	CharType;
typedef std::wstring String;

#define _JEBE_BUCKET_BITS 20
template<uint8_t plen, uint8_t bits>
class PhraseHash;

template<uint8_t length>
class Phrase
{

	template<uint8_t len> friend bool operator==(const Phrase<len>& lph, const Phrase<len>& rph);
//	template<uint8_t len_1, uint8_t len_2> friend bool operator==(const Phrase<len_1>& lph, const Phrase<len_2>& rph);
public:
	typedef boost::unordered_map<Phrase<length>, atimes_t, PhraseHash<length, _JEBE_BUCKET_BITS> > MapType;

	static const uint8_t len = length;
	typedef CharType StrType[length];

	StrType str;

public:
	explicit Phrase(const CharType* const str_)
	{
		std::memcpy(str, str_, length * sizeof(CharType));
	}

	// half-hash. one day, boost::preprocessor ...
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
			const char* str_ = reinterpret_cast<const char*>(str);
			for (uint8_t i = 0, end = length * sizeof(CharType); i < end; ++i)
			{
				res[i & 3] ^= str_[i];
			}
			return mask;
		}
	}

	bool eq(const Phrase<length>& rph) const
	{
		return match(str, rph.str);
	}

	template<uint8_t rstr_len>
	bool __match(const CharType rstr[rstr_len]) const
	{
//		BOOST_STATIC_ASSERT(rstr_len >= length);
		switch (length)
		{
		case 2:
			return str[0] == rstr[0]
				 && str[1] == rstr[1];
		case 3:
			return str[0] == rstr[0]
				 && str[1] == rstr[1]
				 && str[2] == rstr[2];
		case 4:
			return str[0] == rstr[0]
				 && str[1] == rstr[1]
				 && str[2] == rstr[2]
				 && str[3] == rstr[3];
		case 5:
			return str[0] == rstr[0]
				 && str[1] == rstr[1]
				 && str[2] == rstr[2]
				 && str[3] == rstr[3]
				 && str[4] == rstr[4];
		default:
			for (uint8_t i = 0; i < length; ++i)
			{
				if (str[i] != rstr[i])
				{
					return false;
				}
			}
			return true;
		}
	}

	bool hasPrefix(const Phrase<length - 1>& p) const
	{
		return match(p.str, str);
	}

	// NOTE: debug only
	CharType* c_str() const
	{
		CharType* cstr = new CharType[length + 1];
		std::memcpy(cstr, str, length * sizeof(CharType));
		return cstr;
	}
};

template<uint8_t len_1, uint8_t len_2>
bool match(const CharType prefix[len_1], const CharType rstr[len_2])
{
//		BOOST_STATIC_ASSERT(rstr_len >= length);
	switch (len_1)
	{
	case 2:
		return prefix[0] == rstr[0]
			 && prefix[1] == rstr[1];
	case 3:
		return prefix[0] == rstr[0]
			 && prefix[1] == rstr[1]
			 && prefix[2] == rstr[2];
	case 4:
		return prefix[0] == rstr[0]
			 && prefix[1] == rstr[1]
			 && prefix[2] == rstr[2]
			 && prefix[3] == rstr[3];
	case 5:
		return prefix[0] == rstr[0]
			 && prefix[1] == rstr[1]
			 && prefix[2] == rstr[2]
			 && prefix[3] == rstr[3]
			 && prefix[4] == rstr[4];
	default:
		for (uint8_t i = 0; i < len_1; ++i)
		{
			if (prefix[i] != rstr[i])
			{
				return false;
			}
		}
		return true;
	}
}

template<uint8_t len>
bool operator==(const Phrase<len>& lph, const Phrase<len>& rph)
{
	return match<len, len>(lph.str, rph.str);
}

//template<uint8_t len_1, uint8_t len_2>
//bool operator==(const Phrase<len_1>& lph, const Phrase<len_2>& rph)
//{
//	return match<len_1, len_2>(lph.str, rph.str);
//}

template<uint8_t plen, uint8_t bits>
class PhraseHash
{
public:
	typedef Phrase<plen> Ph;
	typedef staging::BitsHash<bits> BHasher;

	static BHasher hasher;

	uint32_t operator()(const Ph& ph) const
	{
		return hasher(ph.hfhash());
	}
};

//typedef std::wstring String;
typedef Phrase<2u> Ph2;
typedef Phrase<3u> Ph3;
typedef Phrase<4u> Ph4;
typedef Phrase<5u> Ph5;
typedef boost::unordered_map<Ph2, atimes_t, PhraseHash<2u, _JEBE_BUCKET_BITS> > Ph2Map;
typedef boost::unordered_map<Ph3, atimes_t, PhraseHash<3u, _JEBE_BUCKET_BITS> > Ph3Map;
typedef boost::unordered_map<Ph4, atimes_t, PhraseHash<4u, _JEBE_BUCKET_BITS> > Ph4Map;
typedef boost::unordered_map<Ph5, atimes_t, PhraseHash<5u, _JEBE_BUCKET_BITS> > Ph5Map;

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
	void extract(const boost::filesystem::path& file, uint32_t max_chars = 0);

	void scan(const CharType* const str, String::size_type len);

	template<uint8_t plen>
	void scanSentence(const CharType* const str, String::size_type len,
			boost::unordered_map<Phrase<plen>, atimes_t, PhraseHash<plen, _JEBE_BUCKET_BITS> >& phmap);

	bool isGb2312(CharType c) const
	{
		return c < gb_char_max && gb2312[c];
	}

	void display();

	void extract();

	Extractor(const boost::filesystem::path& gbfile);
	virtual ~Extractor();
};

class Analyzer
{
protected:
	const Ph2Map& map2;
	const Ph3Map& map3;
	const Ph4Map& map4;
	const Ph5Map& map5;

	static const double entropyThreshold = .5;
	static const double joinThreshold = 30;

public:
	Analyzer(const Ph2Map& map2_, const Ph3Map& map3_, const Ph4Map& map4_, const Ph5Map& map5_)
		: map2(map2_), map3(map3_), map4(map4_), map5(map5_)
	{
	}

	void analysis();

	template<uint8_t plen>
	uint32_t count(const Phrase<plen>& ph) const
	{
		switch (plen)
		{
		case 2:
			return map2[ph];
		case 3:
			return map3[ph];
		case 4:
			return map4[ph];
		case 5:
			return map5[ph];
		default:
			return 0;
		}
	}

	bool wordJudge() const;

	template<uint8_t plen>
	void getSuffixCount(const Phrase<plen>& ph,
			const typename Phrase<plen>::MapType& map,
			const typename Phrase<plen + 1>::MapType& suffixMap
			) const
	{

	}
};

} /* namespace cws */
} /* namespace jebe */
