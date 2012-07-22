
#include "session.hpp"
#include <boost/algorithm/string/find.hpp>
#include "filter.hpp"
#include "memory.hpp"
#include "worker.hpp"
#include "numcast.hpp"

namespace jebe {
namespace cws {

_JEBE_MAKE_HEADER(200, Session);
_JEBE_MAKE_HEADER(400, Session);
const byte_t Session::httpsep[] = _JEBE_HTTP_LINE_SEP;
const byte_t Session::bodysep[] = _JEBE_HTTP_SEP;
const byte_t Session::content_length[] = _JEBE_HTTP_LINE_SEP _JEBE_HTTP_CONTENT_LENGTH;
const Session::Matcher Session::httpsep_matcher(httpsep, httpsep + sizeof(httpsep) - 1, equaler);
const Session::Matcher Session::bodysep_matcher(bodysep, bodysep + sizeof(bodysep) - 1, equaler);
const Session::Matcher Session::content_length_matcher(content_length, content_length + sizeof(content_length) - 1, equaler);

std::size_t Session::header_max_len = 0;
std::size_t Session::body_max_len = 0;
std::size_t Session::timeout = 0;
std::size_t Session::max_write_times = 0;

std::size_t Session::chunkRate;
std::size_t Session::chunkSize;

void Session::handle_read(const boost::system::error_code& error,
    const std::size_t bytes_transferred)
{
    if (CS_BLIKELY(!error))
    {
    	if (CS_BLIKELY(header_end == 0))
    	{
			Range range(
				request + (transferred < CS_CONST_STRLEN(_JEBE_HTTP_SEP) ? 0 : (transferred - CS_CONST_STRLEN(_JEBE_HTTP_SEP))),
				request + std::min(transferred + bytes_transferred, header_max_len)
			);
			Range res = boost::algorithm::find(range, bodysep_matcher);
			if (CS_LIKELY(res))
			{
				header_end = res.begin() - request;
			}
    	}
        transferred += bytes_transferred;

        if (CS_BUNLIKELY(header_end == 0))
        {
			start_receive(transferred);
        }
        else
        {
        	if (CS_BLIKELY(content_length_begin == 0))
        	{
        		Range range(request, request + header_end);
        		Range res = boost::algorithm::find(range, content_length_matcher);
        		if (CS_LIKELY(res))
        		{
        			content_length_begin = res.end() - request;
        			byte_t* clbegin = request + content_length_begin;
        			body_len = staging::NumCast::strtoul(clbegin);
					if (CS_BUNLIKELY(body_len == 0 || body_len > body_max_len))
					{
						finish();
					}
        		}
        	}
            if (CS_BUNLIKELY(body_len) == 0)
            {
                start_receive(bytes_transferred);
            }
            else
            {
				std::size_t body_begin = header_end + (sizeof(bodysep) - 1),
						required = body_begin + body_len;
				if (CS_BUNLIKELY(transferred > required))
				{
					finish();
				}
				else if (CS_BUNLIKELY(transferred != required))
				{
					// should optimize io-times.
					if (CS_BLIKELY(transferred < chunkSize))
					{
						start_receive(transferred);
					}
					else
					{
						tsize_t remains = handler.handle(request + body_begin, transferred - body_begin);
						if (CS_BLIKELY(remains <= chunkSize))
						{
							start_receive(remains);
						}
						else
						{
							finish();	// the /<action> is wrong.
						}
					}
				}
				else
				{
					handler.handle(request + body_begin, transferred - body_begin);
					reply();
				}
            }
        }
    }
}

void Session::finish_by_wait(const boost::system::error_code& error)
{
    finish(error);
}

void Session::config()
{
    header_max_len = Config::getInstance()->header_max_size;
    body_max_len = Config::getInstance()->body_max_size;
    timeout = (Config::getInstance()->timeout > 0) ? Config::getInstance()->timeout : 0;
    max_write_times = (0 < Config::getInstance()->max_write_times) ? Config::getInstance()->max_write_times : 10;

    chunkRate = Config::getInstance()->receive_buffer_size;
    chunkSize = chunkRate * _JEBE_BUFF_UNIT;

    SendBuff::config();
    RequestHandler::init(chunkSize);
}

void Session::start_receive(std::size_t offset)
{
	if (CS_BLIKELY(++write_times < max_write_times))
	{
		sock.async_read_some(
			boost::asio::buffer(request + offset, chunkSize - offset),
			boost::bind(&Session::handle_read, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred
			)
		);
	}
}

Session::Session(boost::asio::io_service& io)
	: sock(io), transferred(0), write_times(0), header_end(0), content_length_begin(0), body_len(0),
	  request(reinterpret_cast<byte_t*>(RecvBuffAlloc::ordered_malloc(chunkRate))),
	  response(header_200, sizeof(header_200) - 1), handler(request, response)
#if _JEBE_USE_TIMER
	,timer(io)
#endif
{
	CS_SAY("session constructed");
}

}
}
