
#include "session.hpp"
#include <boost/algorithm/string/find.hpp>
#include "filter.hpp"
#include "memory.hpp"
#include "worker.hpp"
#include "numcast.hpp"

namespace jebe {
namespace cws {

const Filter* filter = NULL;

const unsigned char* s = reinterpret_cast<unsigned char>("");

_JEBE_MAKE_HEADER(200, Session);
_JEBE_MAKE_HEADER(400, Session);
const std::string Session::httpsep(_JEBE_HTTP_SEP);
const std::string Session::contentLength(_JEBE_HTTP_LINE_SEP _JEBE_HTTP_CONTENT_LENGTH);
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
    	boost::iterator_range<const char*> range(
			request + (transferred < CS_CONST_STRLEN(_JEBE_HTTP_SEP) ? 0 : (transferred - CS_CONST_STRLEN(_JEBE_HTTP_SEP))),
			request + std::min(transferred + bytes_transferred, header_max_len)
		);
    	boost::iterator_range<const char*> httpsep_itr(httpsep.begin(), httpsep.end());
    	std::string s;
    	boost::algorithm::find_first(
    			s,
    			"ffdf");
//        pstr::size_type header_len = request.find(httpsep,
//			CS_BLIKELY(transferred < CS_CONST_STRLEN(_JEBE_HTTP_SEP)) ?
//				0 : (transferred - CS_CONST_STRLEN(_JEBE_HTTP_SEP))
//		);
        transferred += bytes_transferred;

        std::size_t body_len = 0;
        if (CS_BUNLIKELY(header_len == pstr::npos))
        {
            if (CS_BUNLIKELY(++write_times > max_write_times))
            {
                finish();
            }
            else
            {
                start_receive(transferred);
            }
        }
        else
        {
        	pstr::size_type clpos = request.find(contentLength, 0);
            if (CS_BUNLIKELY(clpos == pstr::npos))
            {
                start_receive(bytes_transferred);
            }
            else
            {
                clpos += contentLength.size();
                pstr::size_type clend = request.find(httpsep, clpos);//, CS_CONST_STRLEN(_JEBE_HTTP_LINE_SEP));
                if (CS_BUNLIKELY(clend == pstr::npos))
                {
                    finish();
                }
                else
                {
                	body_len = staging::NumCast::strtoul(request.data() + clpos, clend - clpos);
                	std::size_t required = header_len + body_len + CS_CONST_STRLEN(_JEBE_HTTP_SEP);
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
                        if (CS_BUNLIKELY(max_write_times < ++write_times))
                        {
                            finish();
                        }
                        else
                        {
                            start_receive(transferred);
                        }
                    }
                    else
                    {
                        pstr::size_type body_pos = header_len + CS_CONST_STRLEN(_JEBE_HTTP_SEP);
//                        pstr content(request, body_pos, body_len);

                        tsize_t res_size = filter->filt(reinterpret_cast<const byte_t*>(request.data() + body_pos), body_len, res);
                        response.assign(_JEBE_HEADER(200));
                        response.append(boost::lexical_cast<pstr>(res_size));
                        response.append(httpsep);
                        response.append(res, res_size);
                        reply();
                        finish();
                        boost::iterator_range<char*> it;
                        boost::algorithm::find_first(reinterpret_cast<char*>(const_cast<char*>(response)), "f");
                    }
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
    sock.async_read_some(
        boost::asio::buffer(const_cast<char*>(request.data()) + offset, max_len - offset),
        boost::bind(&Session::handle_read, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred
        )
    );
}

Session::Session(boost::asio::io_service& io)
	: sock(io), transferred(0), write_times(0),
	  request(Config::getInstance()->receive_buffer_size, 0),
	  res(reinterpret_cast<byte_t*>(ResBuffAlloc::ordered_malloc(Config::getInstance()->send_buffer_size))),
	  response(Config::getInstance()->send_buffer_size, 0)
#if _JEBE_USE_TIMER
	,timer(io)
#endif
{
	boost::algorithm::find_first(response, "f");
}

}
}
