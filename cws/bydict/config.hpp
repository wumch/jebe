
#pragma once

#include "../../staging/singleton.hpp"
#include "predef.hpp"

namespace jebe {
namespace cws {

namespace G {
extern const char* config_file;
}

class Config
{
    mksingleton(Config)
public:

    std::string host;
    uint16_t port;
    std::string patten_file;
    std::string pidfile;

    std::size_t worker_cout;
    bool reuse_address;
    std::size_t max_connections;
    bool tcp_nodelay;
    std::size_t receive_buffer_size;
    std::size_t send_buffer_size;
    std::size_t timeout;
    std::size_t max_write_times;
    
    std::size_t header_max_len;
    std::size_t body_max_len;
    std::size_t max_len;
    std::size_t max_match;
    char replacement;

private:
    Config()
    {
        load(G::config_file);
    }

    void load(const char* config_file);
};

namespace G {
extern const Config* config;
}

}
}
