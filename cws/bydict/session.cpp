
#include "session.hpp"
#include <boost/algorithm/string/find.hpp>
#include "filter.hpp"
#include "memory.hpp"
#include "worker.hpp"
#include "numcast.hpp"

namespace jebe {
namespace cws {

const Filter* filter = NULL;

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
std::size_t Session::max_len = 0;
std::size_t Session::timeout = 0;
std::size_t Session::max_write_times = 0;

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

//        std::string::size_type header_pos = request.find(bodysep,
//			CS_BLIKELY(transferred < CS_CONST_STRLEN(_JEBE_HTTP_SEP)) ?
//				0 : (transferred - CS_CONST_STRLEN(_JEBE_HTTP_SEP))
//		);
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
					if (CS_BUNLIKELY(body_len == 0))
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
				std::size_t required = header_end + (sizeof(bodysep) - 1) + body_len;
				if (CS_BUNLIKELY(body_len > Config::getInstance()->body_max_size))
				{
					finish();
				}
				else if (CS_BUNLIKELY(required < transferred))
				{
					finish();
				}
				else if (CS_BUNLIKELY(transferred != required))
				{
					start_receive(transferred);
				}
				else
				{
					tsize_t body_begin = header_end + (sizeof(bodysep) - 1);

//					tsize_t res_size =
					filter->filt(request + body_begin, body_len, response);
//					response.assign(_JEBE_HEADER(200));
//					response.append(boost::lexical_cast<std::string>(res_size));
//					response.append(bodysep);
//					response.append(res, res_size);
					reply();
					finish();
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
    max_len = Session::header_max_len + Session::body_max_len + CS_CONST_STRLEN(_JEBE_HTTP_SEP);
    timeout = (Config::getInstance()->timeout > 0) ? Config::getInstance()->timeout : 0;
    max_write_times = (0 < Config::getInstance()->max_write_times) ? Config::getInstance()->max_write_times : 10;
}

void Session::start_receive(std::size_t offset)
{
	if (CS_BUNLIKELY(max_write_times < ++write_times))
	{
		finish();
	}
	else
	{
		sock.async_read_some(
			boost::asio::buffer(request + offset, Config::getInstance()->receive_buffer_size - offset),
			boost::bind(&Session::handle_read, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred
			)
		);
	}
}

Session::Session(boost::asio::io_service& io)
	: sock(io), transferred(0), write_times(0), header_end(0), content_length_begin(0), body_len(0),
	  request(reinterpret_cast<byte_t*>(RecvBuffAlloc::malloc())),
	  res(NULL)
#if _JEBE_USE_TIMER
	,timer(io)
#endif
{
}

}
}
