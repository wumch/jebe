
#pragma once

#include "predef.hpp"
#include <boost/pool/pool_alloc.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/noncopyable.hpp>
#include <vector>
#include "config.hpp"
#include "numcast.hpp"

namespace jebe {
namespace cws {

class SendBuff
	: boost::noncopyable
{
public:
	typedef std::vector<byte_t*> ChunkList;
	typedef std::vector<boost::asio::const_buffer> BufferList;

protected:
	typedef boost::singleton_pool<staging::CSUnit<2>, sizeof(byte_t) * _JEBE_BUFF_UNIT,
		boost::default_user_allocator_new_delete, boost::details::pool::default_mutex,
		_JEBE_SESS_POOL_INC_STEP> Alloc;
	BOOST_STATIC_ASSERT(sizeof(byte_t) == 1);

protected:
	static tsize_t chunkSize;
	static tsize_t chunkRate;
	static uint32_t httpsep;

	ChunkList chunkList;
	BufferList bufferList;
	tsize_t lastSize;

	const byte_t* const header;
	tsize_t header_size;

	byte_t content_length[32];

public:
	static void config()
	{
		chunkRate = Config::getInstance()->send_buffer_size;
		chunkSize = chunkRate * _JEBE_BUFF_UNIT;
		httpsep = *reinterpret_cast<const uint32_t*>(_JEBE_HTTP_SEP _JEBE_HTTP_SEP);
		Alloc::ordered_free(Alloc::ordered_malloc(chunkRate), chunkRate);		// to prepare memory pool before any request reaches.
	}

public:
	template<typename char_t>
	void write(char_t c)
	{
		if (CS_BUNLIKELY(lastSize + sizeof(char_t) > chunkSize))
		{
			grow();
		}
		*reinterpret_cast<char_t*>(cursor()) = c;
		lastSize += sizeof(char_t);
//		CS_SAY(cursor() - lastSize);
	}

	template<typename char_t>
	void write(const char_t* const bytes, tsize_t n)
	{
		if (CS_BLIKELY(lastSize + n * sizeof(char_t) <= chunkSize))
		{
			memcpy(cursor(), bytes, n * sizeof(char_t));
//			CS_SAY(cursor());
			lastSize += n * sizeof(char_t);
//			CS_SAY(cursor() - lastSize);
		}
		else
		{
			growWrite(reinterpret_cast<const byte_t*>(bytes), n * sizeof(char_t));
			CS_SAY(cursor() - lastSize);
		}
	}

	void backspace(byte_t byte)
	{
		byte_t* end = cursor();
		*(end - 1) = byte;
	}

	void insertNumber(const uint32_t number)
	{
		if (CS_BLIKELY(lastSize + CS_CONST_STRLEN(BOOST_PP_STRINGIZE(INT_MAX)) <= chunkSize))
		{
			lastSize += staging::NumCast::ultostr(number, cursor());
		}
		else
		{
			// `content_length` must not be used at this time.
			write(content_length, staging::NumCast::ultostr(number, content_length));
		}
//		CS_SAY(cursor() - lastSize);
	}

	byte_t* cursor() const
	{
		return *(chunkList.end() - 1) + lastSize;
	}

	tsize_t remains() const
	{
		return chunkSize - lastSize;
	}

	const BufferList& getBuffers()
	{
		bufferList.reserve(chunkList.size() + 2);
		bufferList.push_back(boost::asio::const_buffer(header, header_size));
		attachContentLength();

		ChunkList::const_iterator last = chunkList.end() - 1;
		for (ChunkList::const_iterator it = chunkList.begin(); it != last; ++it)
		{
			bufferList.push_back(boost::asio::const_buffer(*it, chunkSize));
		}
		bufferList.push_back(boost::asio::const_buffer(*last, lastSize));
		return bufferList;
	}

	explicit SendBuff(const byte_t* const header_, tsize_t header_size_)
		: lastSize(0), header(header_), header_size(header_size_)
	{
//		CS_SAY("[" << header << "]");
		grow();
	}

	~SendBuff()
	{
		for (ChunkList::iterator it = chunkList.begin(); it != chunkList.end(); ++it)
		{
			Alloc::ordered_free(*it, chunkRate);
		}
	}

	std::size_t size() const
	{
		if (CS_BLIKELY(chunkList.size() == 1))
		{
			return lastSize;
		}
		else
		{
			std::size_t size_ = lastSize;
			for (ChunkList::const_iterator it = chunkList.begin(), end = chunkList.end() - 1; it != end; ++it)
			{
				size_ += chunkSize;
			}
			return size_;
		}
	}

	bool empty() const
	{
		return !(lastSize > 0 || chunkList.size() > 1);
	}

protected:
	void growWrite(const byte_t* const bytes, tsize_t n)
	{
		tsize_t brk = chunkSize - lastSize, remaining = n - brk;
		memcpy(cursor(), bytes, brk);
		if (CS_BLIKELY(remaining <= chunkSize))
		{
			grow();
		}
		else
		{
//			CS_SAY("rescursion grow-write");
			growWrite(bytes + brk, remaining);	// TODO: solve memory leaks.
		}
		memcpy(cursor(), bytes + brk, remaining);
		lastSize = remaining;
	}

	void grow()
	{
//		CS_SAY("grow for " << this);
		chunkList.push_back(reinterpret_cast<byte_t*>(Alloc::ordered_malloc(chunkRate)));
		lastSize = 0;
	}

	void grow(std::size_t n)
	{
		if (CS_BLIKELY(n == 1))
		{
			grow();
		}
		else
		{
			grow_(n);
		}
	}

	void grow_(std::size_t n)
	{
		for (std::size_t i = 0; i < n; ++i)
		{
			grow();
		}
	}

	void attachContentLength()
	{
		std::size_t len = staging::NumCast::ultostr(size(), content_length);
		*reinterpret_cast<uint32_t*>(content_length + len) = httpsep;
		bufferList.push_back(boost::asio::const_buffer(content_length, len + sizeof(httpsep)));
	}
};

}
}
