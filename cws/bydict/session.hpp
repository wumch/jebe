
#pragma once

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/assert.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/array.hpp>

#include "predef.hpp"
#include "../../staging/singleton.hpp"
#include "shareinfo.hpp"
#include "filter.hpp"

namespace jebe {
namespace cws {

extern const Filter* filter;

extern void destroySock(Sock& sock);
extern void destroySock(SockPtr& sock);

#if defined(USE_POOL) && USE_POOL
class Session;
typedef Creator<Session, 1 << 12, 1 << 6> SessCreator;
#endif

class Session:
#if defined(USE_POOL) && USE_POOL
    public SessCreator,
#endif
    public boost::enable_shared_from_this<Session>,
    public boost::noncopyable
{
public:
#if defined(USE_POOL) && USE_POOL
    using SessCreator::allocator;
#endif
    typedef BA::mutable_buffers_1 ReadBuf;

    explicit Session()
        : scene(0), sock(G::mio),
        request(G::config->request_max_size, 0), max_match(G::config->max_match),
        transferred(0), write_times(0),
        timer(sock.get_io_service(), boost::posix_time::millisec(timeout))
    {
    }

    explicit Session(Sock& _sock)
        : scene(0), sock(_sock.get_io_service()), 
        request(G::config->request_max_size, 0), max_match(G::config->max_match),
        transferred(0), write_times(0),
        timer(sock.get_io_service(), boost::posix_time::millisec(timeout))
    {
    }

    explicit Session(BA::io_service& io)
        : scene(0), sock(io), 
        request(G::config->request_max_size, 0), max_match(G::config->max_match),
        transferred(0), write_times(0),
        timer(io)
    {
    }

    inline Sock& getSock()
    {
        return sock;
    }

    void inline start();
    
    static std::size_t body_max_len;
    static std::size_t header_max_len;
    static std::size_t max_len;
    static std::size_t timeout;
    static std::size_t max_write_times;
    
private:
    void inline start_receive();

    void start_receive(const std::size_t offset);

    void handle_read(const BS::error_code& error,
        const std::size_t bytes_transferred);

    void inline finish(const BS::error_code& error = BS::error_code());

    void finish_by_wait(const BS::error_code& error = BS::error_code());

    void inline reply();

    SceneId scene;

    Sock sock;

    std::string request;
    std::string response;
    const uint16_t max_match;
    std::size_t transferred;
    std::size_t write_times;

    BA::deadline_timer timer;
};

typedef boost::shared_ptr<Session> SessPtr;

void inline Session::start()
{
    start_receive();
    timer.expires_from_now(boost::posix_time::millisec(timeout));
    timer.async_wait(
        boost::bind(&Session::finish_by_wait, shared_from_this(), BA::placeholders::error)
    );
}

void inline Session::start_receive()
{
    sock.async_read_some(
        BA::buffer(const_cast<char*>(request.data()), max_len),
        boost::bind(&Session::handle_read, shared_from_this(),
            BA::placeholders::error,
            BA::placeholders::bytes_transferred
        )
    );
}

void inline Session::start_receive(const std::size_t offset)
{
    sock.async_read_some(
        BA::buffer(const_cast<char*>(request.data()) + offset, max_len - offset),
        boost::bind(&Session::handle_read, shared_from_this(),
            BA::placeholders::error,
            BA::placeholders::bytes_transferred
        )
    );
}

void inline Session::finish(const BS::error_code& error)
{
    if (!error)
    {
        timer.cancel();
        boost::system::error_code ignored_ec;
        sock.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
    }
}

void inline Session::reply()
{
    BA::async_write(sock,
        BA::buffer(response, response.size()),
        boost::bind(&Session::finish, shared_from_this(),
            BA::placeholders::error
        )
    );
}

}
}
