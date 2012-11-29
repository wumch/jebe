
#pragma once

#include "predef.hpp"
#include "singleton.hpp"
#include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/dynamic_bitset.hpp>

extern int main(int, char*[]);

namespace jebe {
namespace cluster {
namespace kmeans {

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
    boost::filesystem::path pattenfile;
    boost::filesystem::path centers_outputfile;
    boost::filesystem::path cls_vecs_outputfile;
    boost::filesystem::path logfile;

    std::string output_delimiter;

    int loglevel;

    const char* argv_first;
    std::string program_name;
    std::size_t io_threads;
    std::size_t collector_num;
    std::size_t calculater_num;
    std::size_t stack_size;

    bool reuse_address;
    std::size_t max_connections;
    bool tcp_nodelay;
    std::size_t receive_buffer_size;
    std::size_t send_buffer_size;
    std::size_t timeout;

    std::size_t msg_max_size;
    std::size_t doc_packer_buf_size;

    std::size_t max_tmp_match;
    std::size_t max_match;

    std::string mongo_server;
    std::string mongo_collection;
    std::string mongo_field;
    bool mongo_auto_reconnect;

    vnum_t mongo_max_doc;

    uint32_t chunk_size;
    uint32_t chunk_num;

    // for performance
    fnum_t reserve_fnum;		// 缺省预留特征数（内存预分配）

    decimal_t min_members_by_avg;		// cluster.min-members / avg-members, suggestion: 0.05
    decimal_t max_members_by_avg;		//         max									  20
    decimal_t max_decompose;
    decimal_t max_separate;

    decimal_t center_min_features_rate;

    vnum_t auto_recalc_threshold;		// threshold (count of member) of auto recalculate the center of a cluster..

    clsnum_t supposed_lowest_k;		// 最底层 类别数
    decimal_t level_k_rate;			// 上层类别数 / 下层类别数
    uint32_t supposed_levels;		// 期望层次数
    decimal_t supposed_k_before_decompose;	// 执行分解前的k / 期望的k

    uint32_t max_iter_times;		// 每个level最大迭代次数

    uint32_t top_level_min_clses;

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

} /* namespace kmeans */
} /* namespace cluster */
} /* namespace jebe */
