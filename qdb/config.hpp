
#pragma once

#include "predef.hpp"
#include "singleton.hpp"
#include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/dynamic_bitset.hpp>

extern int main(int, char*[]);

namespace jebe {
namespace qdb {

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
    std::string listen;
    std::string internal;

    std::string dbpath;
    boost::filesystem::path pidfile;

    std::string program_name;
    std::size_t worker_count;
    std::size_t io_threads;
    std::size_t stack_size;

    bool reuse_address;
    std::size_t max_connections;
    bool tcp_nodelay;
    std::size_t receive_buffer_size;
    std::size_t send_buffer_size;
    std::size_t timeout;
    
    std::size_t msg_max_size;

    std::size_t max_tmp_match;
    std::size_t max_match;

    std::size_t max_open_files;
    std::size_t block_size;
    std::size_t block_cache;
    std::size_t write_buffer_size;

    std::size_t max_retrieve_elements;

    bool memlock;
    boost::dynamic_bitset<> cpuaffinity;

    boost::program_options::variables_map options;
    boost::program_options::options_description desc;

    void initDesc();

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
