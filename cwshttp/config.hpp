
#pragma once

#include "predef.hpp"
#include "singleton.hpp"
#include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/dynamic_bitset.hpp>

extern int main(int, char*[]);

namespace jebe {
namespace cws {

class Config
{
	friend int ::main(int, char*[]);
	template<typename T> friend T* staging::getInstance();
public:
	CS_FORCE_INLINE static const Config* getInstance()
	{
		return staging::getInstance<Config>();
	}

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
    std::size_t msg_max_size;
    std::size_t request_max_size;
    std::size_t max_match;

    bool memlock;
    boost::dynamic_bitset<> cpuaffinity;

    boost::program_options::variables_map options;
    boost::program_options::options_description desc;

    void initDesc();

    static void initialize()
    {

    }

protected:
	CS_FORCE_INLINE static Config* getInst()
	{
		return staging::getInstance<Config>();
	}

    void init(int argc, char* argv[]);

    Config(): desc("allowed config options") {}

    void load(const std::string& config_file);
};

}
}
