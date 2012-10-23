
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
namespace rel {

void Config::init(int argc, char* argv[])
{
	boost::program_options::options_description cmdDesc("allowed options");

	boost::filesystem::path program_path = argv[0];
#if BOOST_VERSION > 104200
	program_name = program_path.filename().string();
#else
	program_name = program_path.filename();
#endif

	std::string default_confile("etc/") ;
	default_confile += program_name + ".conf";
	cmdDesc.add_options()
		("help,h", "show this help and exit.")
		("config,c", boost::program_options::value<std::string>()->default_value(default_confile),
			(std::string("config file, defaults to ") + default_confile).c_str())
	;

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
}

void Config::initDesc()
{
	std::string default_pidfile(std::string("/var/run/") + program_name + ".pid");
	std::string default_listen(std::string("tcp://") + staging::getLanIP() + ":10021");
	desc.add_options()
		("listen", boost::program_options::value<typeof(listen)>()->default_value(default_listen))
		("internal", boost::program_options::value<typeof(listen)>()->default_value("inproc://qdb"),
				"internal communicate address, defaults to `inproc://qdb`")
		("pid-file", boost::program_options::value<typeof(pidfile)>()->default_value(default_pidfile),
				(std::string("pid file, defaults to ") + default_pidfile).c_str())
		("patten-file", boost::program_options::value<typeof(pattenfile)>()->default_value("etc/patten.txt"))
		("output-file", boost::program_options::value<typeof(outputfile)>()->default_value("etc/words.txt"))
		("receive-buffer-size", boost::program_options::value<typeof(receive_buffer_size)>()->default_value((64)),
				"siz eof receive buffer (in bytes), default is 4KB")
		("send-buffer-size", boost::program_options::value<typeof(send_buffer_size)>()->default_value((4)),
				"size of send buffer (in bytes), default is 4KB")
		("memlock", boost::program_options::value<typeof(memlock)>()->default_value(false))
		("stack-size", boost::program_options::value<typeof(stack_size)>()->default_value(staging::getRlimitCur(RLIMIT_STACK)))
		("cpuaffinity", boost::program_options::value<std::string>()->default_value(""))
		("worker-count", boost::program_options::value<typeof(worker_count)>()->default_value(staging::getCpuNum() - 1))
		("io-threads", boost::program_options::value<typeof(io_threads)>()->default_value(1))
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
	listen = options["listen"].as<typeof(listen)>();
	internal = options["internal"].as<typeof(internal)>();
	pidfile = options["pid-file"].as<typeof(pidfile)>();
	receive_buffer_size = options["receive-buffer-size"].as<typeof(receive_buffer_size)>() << 10;
	send_buffer_size = options["send-buffer-size"].as<typeof(send_buffer_size)>() << 10;
	worker_count = options["worker-count"].as<typeof(worker_count)>();
	io_threads = options["io-threads"].as<typeof(io_threads)>();

	memlock = options["memlock"].as<typeof(memlock)>();
	if (memlock)
	{
#if CS_DEBUG && !NDEBUG
		int res = mlockall(MCL_CURRENT | MCL_FUTURE);
		assert(!res);
#else
		mlockall(MCL_CURRENT | MCL_FUTURE);
#endif
	}

	CS_SAY("configs in <" << config_file << ">:" << std::endl
		_JEBE_OUT_CONFIG_PROPERTY(listen)
		_JEBE_OUT_CONFIG_PROPERTY(internal)
		_JEBE_OUT_CONFIG_PROPERTY(pidfile)
		_JEBE_OUT_CONFIG_PROPERTY(worker_count)
		_JEBE_OUT_CONFIG_PROPERTY(io_threads)
		_JEBE_OUT_CONFIG_PROPERTY(stack_size)
		_JEBE_OUT_CONFIG_PROPERTY(receive_buffer_size)
		_JEBE_OUT_CONFIG_PROPERTY(send_buffer_size)
	);
}

}
}

#undef _JEBE_OUT_CONFIG_PROPERTY
