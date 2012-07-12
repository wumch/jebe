
#pragma once

#include "staging.hpp"
#include <vector>
#include <algorithm>
#include <bitset>
#include <list>
#include <iostream>
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
	typedef Phrase<length> P;

	typedef boost::unordered_map<P, atimes_t, PhraseHash<length, _JEBE_BUCKET_BITS> > MapType;

	typedef Phrase<1> Suffix;
	typedef std::pair<Suffix, atimes_t> Pad;

	template<typename T>
	class SumedList
	{
	public:
		std::size_t sum;
		typedef std::list<T> List;
		List list;

		typedef typename List::iterator iterator;
		typedef typename List::const_iterator const_iterator;

		SumedList(): sum(0) {}

		void append(const T& elem)
		{
			list.push_back(elem);
			sum += elem.second;
		}

		List* operator->()
		{
			return &list;
		}
	};

	typedef SumedList<Pad> PadList;
	typedef boost::unordered_map<P, PadList, PhraseHash<length, _JEBE_BUCKET_BITS> > PadMap;

	static const uint8_t len = length;
	typedef CharType StrType[length];

//protected:
	StrType str;

public:
	explicit Phrase(const CharType* const str_)
	{
		memcpy(str, str_, length * sizeof(CharType));
	}

	// half-hash. one day, boost::preprocessor ...
	uint32_t hfhash() const
	{
		typedef uint32_t mask_t;
		if (sizeof(CharType) == sizeof(mask_t))
		{
			switch (length)
			{
			case 1:
				return str[0];
			case 2:
				return str[0] ^ str[1];
			case 3:
				return str[0] ^ str[1] ^ str[2];
			case 4:
				return str[0] ^ str[1] ^ str[2] ^ str[3];
			case 5:
				return str[0] ^ str[1] ^ str[2] ^ str[3] ^ str[4];
			case 6:
				return str[0] ^ str[1] ^ str[2] ^ str[3] ^ str[4] ^ str[5];
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

	bool eq(const P& rph) const
	{
		return match(str, rph.str);
	}

	bool hasPrefix(const Phrase<length - 1>& p) const
	{
		return match(p.str, str);
	}

	operator String() const
	{
		return String(c_str());
	}

	// NOTE: debug only
	CharType* c_str() const
	{
		CharType* cstr = new CharType[length + 1];
		memset(cstr, 0, (length + 1) * sizeof(CharType));
		memcpy(cstr, str, length * sizeof(CharType));
		return cstr;
	}
};

template<uint8_t len_1, uint8_t len_2>
bool match(const CharType prefix[len_1], const CharType rstr[len_2])
{
	BOOST_STATIC_ASSERT(len_1 <= len_2);
	switch (len_1)
	{
	case 1:
		return prefix[0] == rstr[0];
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
	case 6:
		return prefix[0] == rstr[0]
			 && prefix[1] == rstr[1]
			 && prefix[2] == rstr[2]
			 && prefix[3] == rstr[3]
			 && prefix[4] == rstr[4]
			 && prefix[5] == rstr[5];
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

typedef Phrase<1> Ph1;
typedef Phrase<2> Ph2;
typedef Phrase<3> Ph3;
typedef Phrase<4> Ph4;
typedef Phrase<5> Ph5;
typedef Phrase<6> Ph6;
//typedef boost::unordered_map<Ph2, atimes_t, PhraseHash<2u, _JEBE_BUCKET_BITS> > Ph2Map;
//typedef boost::unordered_map<Ph3, atimes_t, PhraseHash<3u, _JEBE_BUCKET_BITS> > Ph3Map;
//typedef boost::unordered_map<Ph4, atimes_t, PhraseHash<4u, _JEBE_BUCKET_BITS> > Ph4Map;
//typedef boost::unordered_map<Ph5, atimes_t, PhraseHash<5u, _JEBE_BUCKET_BITS> > Ph5Map;

class Extractor
{
protected:
	static const int32_t gb_char_max = 65536;
	std::bitset<gb_char_max> gb2312;

	Ph1::MapType map1;
	Ph2::MapType map2;
	Ph3::MapType map3;
	Ph4::MapType map4;
	Ph5::MapType map5;
	Ph6::MapType map6;

public:
	void extract(const boost::filesystem::path& file, uint32_t max_chars = 0);

	void scan(const CharType* const str, String::size_type len);

	void addSentence_(const CharType* const str, String::size_type len);

	template<uint8_t plen>
	void scanSentence(const CharType* const str, String::size_type len,
			boost::unordered_map<Phrase<plen>, atimes_t, PhraseHash<plen, _JEBE_BUCKET_BITS> >& phmap);

	bool isGb2312(CharType c) const
	{
		return c >= 0 && c < gb_char_max && gb2312[c];
	}

	void display();

	void extract();

	Extractor(const boost::filesystem::path& gbfile);
	virtual ~Extractor();
};

template<uint8_t prefix_len>
bool padEq(const typename Phrase<prefix_len>::Pad& p1, const typename Phrase<prefix_len>::Pad& p2)
{
	return match<prefix_len, prefix_len>(p1.first.str, p2.first.str);	// Phrase
}

template<uint8_t plen>
class PadEqual
{
private:
	typedef Phrase<plen> P;
	const P& phrase;

public:
	explicit PadEqual(const P& p): phrase(p) {}

	bool operator()(const typename P::Pad& p) const
	{
		return p.first == phrase;
	}
};

class Analyzer
{
protected:
	Ph1::MapType& map1;
	Ph2::MapType& map2;
	Ph3::MapType& map3;
	Ph4::MapType& map4;
	Ph5::MapType& map5;
	Ph6::MapType& map6;

	Ph1::PadMap pad1;		// pad1 -> atimes = map1[prefix] or sum(suffix.atimes) ? should be sum(..).
	Ph2::PadMap pad2;
	Ph3::PadMap pad3;
	Ph4::PadMap pad4;
	Ph5::PadMap pad5;

	std::size_t totalAtimes;

	std::list<std::wstring> words;

	static const double entropyThreshold = 0.5;
	static const double joinThreshold = 10;
	static const double joinThresholdStop = 100;

public:
	Analyzer(Ph1::MapType& map1_, Ph2::MapType& map2_, Ph3::MapType& map3_,
			Ph4::MapType& map4_, Ph5::MapType& map5_, Ph6::MapType& map6_)
		: map1(map1_), map2(map2_), map3(map3_),
		  map4(map4_), map5(map5_), map6(map6_),
		  totalAtimes(0)
	{
	}

	void extractWords()
	{
		extractWords_<2>(map2, map1, pad2);
		extractWords_<3>(map3, map2, pad3);
		extractWords_<4>(map4, map3, pad4);
		extractWords_<5>(map5, map4, pad5);
		CS_SAY("character-total-atimes: " << totalAtimes);
	}

	template<uint8_t plen>
	void extractWords_(typename Phrase<plen>::MapType& map,
			typename Phrase<plen - 1>::MapType& prefixmap,
			typename Phrase<plen>::PadMap& padmap
			)
	{
		BOOST_STATIC_ASSERT(plen > 1);

		for (typename Phrase<plen>::MapType::const_iterator it = map.begin(); it != map.end(); ++it)
		{
			if (isWord<plen>(it->first, map, prefixmap, padmap))
			{
				words.push_back(it->first);
			}
		}
	}

	template<uint8_t plen>
	bool isWord(const Phrase<plen>& phrase,
			typename Phrase<plen>::MapType& map,
			typename Phrase<plen - 1>::MapType& prefixmap,
			typename Phrase<plen>::PadMap& padmap
//			typename Phrase<plen>::PadList& padlist
			) const
	{
		typedef Phrase<plen - 1> PhraseType;
		typedef typename PhraseType::MapType MapType;
		typedef typename PhraseType::PadMap PadMapType;
		typedef typename PhraseType::PadList PadListType;
		typedef typename PhraseType::Suffix SuffixType;

		PhraseType prefix(phrase.str);
		SuffixType suffix(phrase.str + (plen - 1));

//		PadListType& plist = padmap[prefix];
		// both count(prefix) and  can not be 0, it is impossible.
		/*
		double joinprobActual = count(phrase) / count(prefix),
				joinprobPred = count(suffix) / totalAtimes;
		double overRate = joinprobActual / joinprobPred;
		// so, an a bit fast and explicit edition: */
		double atimes = map[phrase];
		double overRate = std::log(atimes) * atimes / prefixmap[prefix] * totalAtimes / map1[suffix];
		if (overRate > joinThresholdStop)
		{
			CS_SAY("must be a word: [" << phrase.c_str() << "], overRate: " << overRate
							<< ", map[phrase]: " << map[phrase]
							<< ", prefixmap[prefix]: " << prefixmap[prefix]
							<< ", map1[suffix]: " << map1[suffix]
							);
			return true;
		}

		if (overRate > joinThreshold)
		{
			CS_SAY("can be a word: [" << phrase.c_str() << "]");
		}
		CS_SAY("can be a word: [" << phrase.c_str() << "], overRate: " << overRate
				<< ", map[phrase]: " << map[phrase]
				<< ", prefixmap[prefix]: " << prefixmap[prefix]
				<< ", map1[suffix]: " << map1[suffix]
				);
		return false;

//		std::log(joinfreq);
	}

	void analysis();

	void caltureTotalAtimes()
	{
		for (Ph1::MapType::const_iterator it = map1.begin(); it != map1.end(); ++it)
		{
			totalAtimes += it->second;
		}
	}

	void buildPadMap()
	{
		buildPadMap_<1>(pad1, map2);
		buildPadMap_<2>(pad2, map3);
		buildPadMap_<3>(pad3, map4);
		buildPadMap_<4>(pad4, map5);
		buildPadMap_<5>(pad5, map6);
	}

	template<uint8_t prefix_len>
	void buildPadMap_(typename Phrase<prefix_len>::PadMap& padmap, const typename Phrase<prefix_len + 1>::MapType& map)
	{
		typedef Phrase<prefix_len> PrefixType;
		typedef typename PrefixType::Suffix SuffixType;
		typedef typename Phrase<prefix_len + 1>::MapType MapType;

		typedef typename PrefixType::PadList PadListType;
		typedef typename PrefixType::PadMap PadMapType;

		for (typename MapType::const_iterator it = map.begin(); it != map.end(); ++it)
		{
			PrefixType prefix(it->first.str);
			SuffixType suffix(it->first.str + prefix_len);

			if (padmap.find(prefix) == padmap.end())
			{
				padmap[prefix] = PadListType();
			}

			PadListType& plist = padmap[prefix];
			typename PadListType::iterator padit =std::find_if(plist->begin(), plist->end(), PadEqual<1>(suffix));
			if (padit == plist->end())
			{
				plist.append(typename PrefixType::Pad(suffix, it->second));
				CS_SAY("first " << prefix_len << ", prefix: [" << prefix.c_str() << "], suffix: [" << suffix.c_str() << "], atimes: " << it->second);
			}
			else
			{
				padit->second = it->second;
				CS_SAY("repeat " << prefix_len << ", prefix: [" << prefix.c_str() << "], suffix: " << suffix.c_str() << "], atimes: " << it->second);
			}
			CS_SAY("plist.sum: " << plist.sum << " (" << &plist << ")");
		}
	}

	void clean(std::size_t min_atimes);

	template<uint8_t plen>
	void clean_(typename Phrase<plen>::MapType& map, std::size_t min_atimes);

	template<uint8_t plen>
	uint32_t count(const Phrase<plen>& ph) const
	{
		switch (plen)
		{
//		case 1:
//			return map1.find(p)->second;
//		case 2:
//			return map2[ph];
//		case 3:
//			return map3[ph];
//		case 4:
//			return map4[ph];
//		case 5:
//			return map5[ph];
//		case 6:
//			return map6[ph];
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
