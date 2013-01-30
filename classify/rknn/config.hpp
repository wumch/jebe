
#pragma once

#include "predef.hpp"
#include "singleton.hpp"
#include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/dynamic_bitset.hpp>

extern int main(int, char*[]);

namespace jebe {
namespace classify {
namespace rknn {

class Config
{
	friend int ::main(int, char*[]);
	friend class Aside;
	template<typename T> friend T* staging::getInstance();
public:
	CS_FORCE_INLINE static const Config* getInstance()
	{
		return staging::getInstance<Config>();
	}

public:
    std::string listen;
    std::string internal;

    boost::filesystem::path pidfile;
    boost::filesystem::path logfile;

    int loglevel;

    const char* argv_first;
    std::string program_name;
    size_t io_threads;
    size_t calculator_num;
    size_t stack_size;

    bool reuse_address;
    std::size_t max_connections;
    bool tcp_nodelay;
    size_t receive_buffer_size;
    size_t send_buffer_size;
    size_t msg_max_size;
    size_t timeout;

    boost::filesystem::path data_dir;
    boost::filesystem::path transformer_file;
    boost::filesystem::path classified_vector_file;
    boost::filesystem::path ropt_file;

    // for performance
    fnum_t reserve_fnum;		// 缺省预留特征数（内存预分配）

    size_t k;
    decimal_t k_rate;			// k = k_rate * ropt_cls_num
    size_t ropt_cls_num;		// num of Rocchio-optimized classes should be picked out for calculate best-class.

    bool memlock;
    boost::dynamic_bitset<> cpuaffinity;
    bool record_on_cache;

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

} /* namespace rknn */
} /* namespace classify */
} /* namespace jebe */
