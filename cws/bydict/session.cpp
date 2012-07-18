
#include "staging.hpp"
#include "memory.hpp"
#include "session.hpp"
#include "filter.hpp"

namespace jebe {
namespace cws {

const Filter* filter = NULL;

std::size_t Session::header_max_len = 0;
std::size_t Session::body_max_len = 0;
std::size_t Session::max_len = 0;
std::size_t Session::timeout = 0;
std::size_t Session::max_write_times = 0;

void Session::handle_read(const BS::error_code& error,
    const std::size_t bytes_transferred)
{
    if (CS_BLIKELY(!error))
    {
        std::string::size_type header_len = request.find(_JEBE_HTTP_SEP, 0);//, CS_CONST_STRLEN(_JEBE_HTTP_SEP));
        transferred += bytes_transferred;
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
                if (CS_BUNLIKELY(clend == std::string::npos))
                {
                    finish();
                }
                else
                {
                    body_len = boost::lexical_cast<std::size_t>(request.substr(clpos, clend - clpos));
                    std::size_t required = header_len + body_len + sizeof(_JEBE_HTTP_SEP) - 1;
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
                        std::string::size_type body_pos = header_len + sizeof(_JEBE_HTTP_SEP) - 1;
                        std::string content(request, body_pos, body_len);

                        std::string res_ = filter->filt(content, res);
                        response.append(HEADER(200));
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

void Session::finish_by_wait(const BS::error_code& error)
{
    finish(error);
}

void destroySock(Sock& sock)
{
    sock.cancel();
    sock.close();
}

void destroySock(SockPtr& sock)
{
    sock->cancel();
    sock->close();
}

}
}
