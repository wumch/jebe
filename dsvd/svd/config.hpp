
#pragma once

#include "predef.hpp"
#include "singleton.hpp"
#include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/logic/tribool.hpp>

extern int main(int, char*[]);

namespace jebe {
namespace dsvd {
namespace svd {

namespace {
BOOST_TRIBOOL_THIRD_STATE(nonspecified)
}

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

private:
	enum {
		cmd_style = boost::program_options::command_line_style::unix_style
			| boost::program_options::command_line_style::allow_long_disguise	// to work around Petsc options.
	};

public:
	int argc;
	char** argv;
	boost::filesystem::path config_file;

	bool as_daemon;
    boost::filesystem::path pidfile;

    boost::filesystem::path logfile;
    int loglevel;
    const char* argv_first;
    std::string program_name;

    int main_process;		// the MPI_Comm_rank() of the main-process, usual.ly 0.

    boost::filesystem::path matfile;

    boost::filesystem::path output_dir;
    std::string matfile_ext;
    std::string solution_file_ext;
    std::string text_file_ext;

    boost::tribool store_USV;		// whether store U/S/V or not.
    boost::filesystem::path outfile_U;
    boost::filesystem::path outfile_S;
    boost::filesystem::path outfile_Vt;

    boost::tribool store_solution;	// whether store the solution of the SVD problem or not.
    boost::filesystem::path outfile_solution;
    boost::filesystem::path outfile_solution_text;

    bool transpose_vt;		// whether transposed V or not. true: output V, false: output Vt.

    bool asc;		// sort singular values ascending or descending. default: false (i.e. ascending).

    size_t r;			// "rank" supposed.
    size_t nsv;			// num of singular values.
    size_t ncv;			// num of column-vectors which join in SVD.

    boost::tribool store_product;
    boost::filesystem::path outfile_SvUt;		// cuted(U) * cuted(S)^-1
    boost::filesystem::path outfile_feature_space;	// Vk (= SvUt * A), the feature space.

    bool memlock;
    boost::dynamic_bitset<> cpuaffinity;

    boost::program_options::variables_map options;
    boost::program_options::options_description desc;

protected:
	CS_FORCE_INLINE static Config* getInst()
	{
		return staging::getInstance<Config>();
	}

    void init(int argc, char* argv[]);

    Config()
    	: store_USV(nonspecified),
    	  store_solution(nonspecified),
    	  store_product(nonspecified),
    	  desc("allowed config options")
    {}

    void load();

private:
    void solve_files();
};

} /* namespace svd */
} /* namespace dsvd */
} /* namespace jebe */
