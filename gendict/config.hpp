
#pragma once

#include "ppredef.hpp"
#include <string>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/dynamic_bitset.hpp>
#include "singleton.hpp"

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
    boost::filesystem::path pidfile;
    boost::filesystem::path pattenfile;
    boost::filesystem::path outputfile;

    typedef std::vector<std::string> FileList;
    FileList inputfiles;
    boost::filesystem::path extension;

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

    Config(): desc("allowed config options") {}

    void load(const std::string& config_file);

private:
    void checkAddInputFile(const boost::filesystem::path& path)
    {
    	if (boost::filesystem::is_regular_file(path))
		{
			if (extension.empty() || path.extension() == extension)
			{
#if BOOST_VERSION > 104200
				inputfiles.push_back(path.string());
#else
				inputfiles.push_back(dit->path());
#endif
				CS_SAY("inputfile: " << path);
			}
		}
    }
};

}
}
