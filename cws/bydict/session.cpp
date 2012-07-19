
#include "session.hpp"
#include "filter.hpp"
#include "memory.hpp"

namespace jebe {
namespace cws {

const Filter* filter = NULL;

_JEBE_MAKE_HEADER(200, Session);
_JEBE_MAKE_HEADER(400, Session);
const std::string Session::httpsep(_JEBE_HTTP_SEP);
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
        std::string::size_type header_len = request.find(
			httpsep, CS_BUNLIKELY(transferred > CS_CONST_STRLEN(_JEBE_HTTP_SEP))
				? (transferred - CS_CONST_STRLEN(_JEBE_HTTP_SEP)) : 0
		);//, CS_CONST_STRLEN(_JEBE_HTTP_SEP));

        std::size_t body_len = 0;
        if (CS_BUNLIKELY(header_len == std::string::npos))
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
            std::string::size_type clpos = request.find(_JEBE_HTTP_CONTENT_LENGTH, 0);//, CS_CONST_STRLEN(_JEBE_HTTP_CONTENT_LENGTH));
            if (CS_BUNLIKELY(clpos == std::string::npos))
            {
                start_receive(bytes_transferred);
            }
            else
            {
                clpos += sizeof(_JEBE_HTTP_CONTENT_LENGTH) - 1;
                std::string::size_type clend = request.find(_JEBE_HTTP_LINE_SEP, clpos);//, CS_CONST_STRLEN(_JEBE_HTTP_LINE_SEP));
                transferred += bytes_transferred;
                if (CS_BUNLIKELY(clend == std::string::npos))
                {
                    finish();
                }
                else
                {
                    body_len = boost::lexical_cast<std::size_t>(request.substr(clpos, clend - clpos));
                    std::size_t required = header_len + body_len + CS_CONST_STRLEN(_JEBE_HTTP_SEP);
                    if (CS_BUNLIKELY(required < transferred))
                    {
                        finish();
                    }
                    else if (CS_BUNLIKELY(transferred < required))
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
                        std::string::size_type body_pos = header_len + CS_CONST_STRLEN(_JEBE_HTTP_SEP);
                        std::string content(request, body_pos, body_len);

                        std::string res_ = filter->filt(content, res);
                        response.append(_JEBE_HEADER(200));
                        response.append(boost::lexical_cast<std::string>(res_.size()));
                        response.append(_JEBE_HTTP_SEP);
                        response.append(res_);
                        reply();
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
    timeout = (0 < Config::getInstance()->timeout) ? Config::getInstance()->timeout : 1;
    max_write_times = (0 < Config::getInstance()->max_write_times) ? Config::getInstance()->max_write_times : 10;
}

void Session::start_receive(const std::size_t offset)
{
    sock.async_read_some(
        boost::asio::buffer(const_cast<char*>(request.data()) + offset, max_len - offset),
        boost::bind(&Session::handle_read, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred
        )
    );
}

}
}
