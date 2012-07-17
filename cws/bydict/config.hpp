
#pragma once

#include "singleton.hpp"
#include "predef.hpp"
#include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>

namespace jebe {
namespace cws {

namespace G {
extern const char* config_file;
}

namespace bo = boost::program_options;

class Config
{
    mksingleton(Config)
public:

    std::string host;
    uint16_t port;
    boost::filesystem::path patten_file;
    boost::filesystem::path pidfile;

    std::string program_name;
    std::size_t worker_count;
    bool reuse_address;
    std::size_t max_connections;
    bool tcp_nodelay;
    std::size_t receive_buffer_size;
    std::size_t send_buffer_size;
    std::size_t timeout;
    std::size_t max_write_times;
    
    std::size_t header_max_size;
    std::size_t body_max_size;
    std::size_t request_max_size;
    std::size_t max_match;
    char replacement;

    boost::program_options::variables_map options;
    boost::program_options::options_description desc;

    void initDesc();

    void init(int argc, const char* const argv[]);

private:
    Config(): desc("allowed config options") {}

    void load(const std::string& config_file);
};

namespace G {
extern const Config* config;
}

}
}
