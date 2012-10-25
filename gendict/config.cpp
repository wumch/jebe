
#ifndef __GNUC__
#	error "before resolving config options, please make sure your compiler has `typeof(var)` supported"
#endif

#include "config.hpp"
#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/version.hpp>
#include <boost/lexical_cast.hpp>
extern "C" {
#include <sched.h>
}
#include "sys.hpp"
#include "net.hpp"

#define _JEBE_OUT_CONFIG_PROPERTY(property)		<< CS_OC_GREEN(#property) << ":\t" << CS_OC_RED(property) << std::endl

namespace jebe {
namespace cws {

void Config::init(int argc, char* argv[])
{
	boost::program_options::options_description cmdDesc("allowed options");
	cmdDesc.add_options()
		("help,h", "show this help and exit.")
		("config,c", boost::program_options::value<std::string>()->default_value("etc/preprocess.conf"),
			"config file, defaults to etc/preprocess.conf.")
	;

	boost::filesystem::path program_path = argv[0];
#if BOOST_VERSION > 104200
	program_name = program_path.filename().string();
#else
	program_name = program_path.filename();
#endif

//	boost::program_options::variables_map cmdOptions;
//	try
//	{
//		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, cmdDesc, boost::program_options::command_line_style::unix_style), cmdOptions);
//	}
//	catch (const std::exception& e)
//	{
//		CS_DIE(e.what());
//	}
//	boost::program_options::notify(cmdOptions);

	boost::program_options::variables_map cmdOptions;
	FileList files;;
	try
	{
		boost::program_options::parsed_options parsed = boost::program_options::command_line_parser(argc, argv).options(cmdDesc).allow_unregistered().run();
		boost::program_options::store(parsed, cmdOptions);
		files = boost::program_options::collect_unrecognized(parsed.options, boost::program_options::include_positional);
	}
	catch (const std::exception& e)
	{
		CS_DIE(e.what());
	}
	boost::program_options::notify(cmdOptions);

	if (cmdOptions.count("help"))
	{
		std::cout << cmdDesc << std::endl;
		std::exit(0);
	}
	else if (cmdOptions.count("config"))
	{
		initDesc();
		load(cmdOptions["config"].as<std::string>());
	}

	for (FileList::const_iterator it = files.begin(); it != files.end(); ++it)
	{
		boost::filesystem::path path(*it);
		if (boost::filesystem::exists(path))
		{
			if (boost::filesystem::is_directory(path))
			{
				boost::filesystem::directory_iterator dend;
				for (boost::filesystem::directory_iterator dit(path); dit != dend; ++dit)
				{
					checkAddInputFile(dit->path());
				}
			}
			else
			{
				checkAddInputFile(path);
			}
		}
		else
		{
			CS_ERR("file/dir non-exists: " << *it);
		}
	}
}

void Config::initDesc()
{
	desc.add_options()
		("pid-file", boost::program_options::value<typeof(pidfile)>()->default_value("/var/run/preprocess.pid"))
		("patten-file", boost::program_options::value<typeof(pattenfile)>()->default_value("etc/patten.txt"))
		("output-file", boost::program_options::value<typeof(outputfile)>()->default_value("etc/words.txt"))
		("input-files", boost::program_options::value<std::string>()->default_value(""))
		("input-file-extension", boost::program_options::value<std::string>()->default_value(""))

		("memlock", boost::program_options::value<typeof(memlock)>()->default_value(false))
		("stack-size", boost::program_options::value<typeof(stack_size)>()->default_value(staging::getRlimitCur(RLIMIT_STACK)))
		;
}

void Config::load(const std::string& config_file)
{
	try
	{
		boost::program_options::store(boost::program_options::parse_config_file<char>(config_file.c_str(), desc), options);
	}
	catch (const std::exception& e)
	{
		CS_DIE("faild on read/parse config-file: " << config_file.c_str() << "\n" << e.what());
	}
	boost::program_options::notify(options);
	pidfile = options["pid-file"].as<typeof(pidfile)>();
	pattenfile = options["patten-file"].as<typeof(pattenfile)>();
	outputfile = options["output-file"].as<typeof(outputfile)>();
	std::string input_files = options["input-files"].as<typeof(std::string)>();
	extension = options["input-file-extension"].as<typeof(std::string)>();

	memlock = options["memlock"].as<typeof(memlock)>();
	if (memlock)
	{
#if CS_DEBUG
		assert(!mlockall(MCL_CURRENT | MCL_FUTURE));
#else
		mlockall(MCL_CURRENT | MCL_FUTURE);
#endif
	}
	stack_size = options["stack-size"].as<typeof(stack_size)>();
	staging::setRlimit(RLIMIT_STACK, stack_size);

	CS_SAY("configs in <" << config_file << ">:" << std::endl
		_JEBE_OUT_CONFIG_PROPERTY(pidfile)
		_JEBE_OUT_CONFIG_PROPERTY(pattenfile)
		_JEBE_OUT_CONFIG_PROPERTY(outputfile)
		_JEBE_OUT_CONFIG_PROPERTY(input_files)
		_JEBE_OUT_CONFIG_PROPERTY(stack_size)
		_JEBE_OUT_CONFIG_PROPERTY(memlock)
	);
}

}
}

#undef _JEBE_OUT_CONFIG_PROPERTY
