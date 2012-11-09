
#pragma once

#include "predef.hpp"
#include <string>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/dynamic_bitset.hpp>
#include "singleton.hpp"

extern int main(int, char*[]);

namespace jebe {
namespace cluster {

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
	decimal_t df_quantile_bottom;		// = 0.6
	decimal_t df_quantile_top;			// = 0.9

	decimal_t min_word_corr;		// min coefficient-of-correlation of a pair of related-words.
	decimal_t max_word_corr;

	decimal_t min_wd_var;
	decimal_t max_wd_var;

	decimal_t wd_var_bottom;
	decimal_t wd_var_top;

    boost::filesystem::path pidfile;
    boost::filesystem::path pattenfile;
    boost::filesystem::path outputfile;
    boost::filesystem::path logfile;

    int loglevel;

    uint32_t io_threads;
    uint32_t worker_count;
    std::string listen;
    std::string internal;

    uint64_t send_buffer_size;
    uint64_t receive_buffer_size;

    uint64_t sim_buff_size;

    typedef std::vector<std::string> FileList;
    FileList inputfiles;
    boost::filesystem::path extension;

    const char* argv_first;
    std::string program_name;
    std::size_t stack_size;
    bool memlock;

    boost::program_options::variables_map options;
    boost::program_options::options_description desc;

    void initDesc();

protected:
	CS_FORCE_INLINE static Config* getInst()
	{
		return staging::getInstance<Config>();
	}

    void init(int argc, char* argv[]);

    Config(): argv_first(NULL), desc("allowed config options") {}

    void load(const std::string& config_file);

private:
    void checkAddInputFile(const boost::filesystem::path& path)
    {
    	if (boost::filesystem::is_regular_file(path))
		{
			if (extension.empty() || path.extension() == extension)
			{
				inputfiles.push_back(path.string());
				CS_SAY("inputfile: " << path);
			}
		}
    }
};

}
}
