
#ifndef __GNUC__
#	error "before resolving config options, please make sure your compiler has `typeof(var)` supported"
#endif

#include "config.hpp"
#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
extern "C" {
#include <sched.h>
}
#include "sys.hpp"
#include "net.hpp"

#define _JEBE_OUT_CONFIG_PROPERTY(property)		<< #property << ":\t\t" << property << std::endl

namespace jebe {
namespace classify {
namespace rknn {

void Config::init(int argc, char* argv[])
{
	argv_first = argv[0];
	boost::filesystem::path program_path = argv_first;
#if BOOST_VERSION > 104200
	program_name = program_path.filename().string();
#else
	program_name = program_path.filename();
#endif

	boost::program_options::options_description cmdDesc("allowed options");
	cmdDesc.add_options()
		("help,h", "show this help and exit.")
		("config,c", boost::program_options::value<std::string>()->default_value("etc/" + program_name + ".conf"),
			("config file, defaults to etc/" + program_name + ".conf.").c_str())
	;

	boost::program_options::variables_map cmdOptions;
	try
	{
		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, cmdDesc, boost::program_options::command_line_style::unix_style), cmdOptions);
	}
	catch (const std::exception& e)
	{
		CS_DIE(e.what() << "\n" << cmdDesc);
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
	boost::filesystem::path default_pidfile(std::string("/var/run/") + program_name + ".pid");
	std::string default_listen(std::string("tcp://") + staging::getLanIP() + ":10025");
	boost::filesystem::path default_logfile(std::string("/var/log/") + program_name + ".log");
	desc.add_options()
		("listen", boost::program_options::value<typeof(listen)>()->default_value(default_listen))
		("internal", boost::program_options::value<typeof(internal)>()->default_value(("inproc://" + program_name)),
			("internal communicate address, defaults to `inproc://" + program_name).c_str())
		("pid-file", boost::program_options::value<typeof(pidfile)>()->default_value(default_pidfile),
			("pid file, defaults to " + default_pidfile.string()).c_str())
		("log-file", boost::program_options::value<typeof(logfile)>()->default_value(default_logfile))
		("log-level", boost::program_options::value<typeof(loglevel)>()->default_value(0))
		("reuse-address", boost::program_options::value<typeof(reuse_address)>()->default_value(true),
			"whether reuse-address on startup or not, default is on")
		("receive-buffer-size", boost::program_options::value<typeof(receive_buffer_size)>()->default_value((64)),
			"siz eof receive buffer (in bytes), default is 64KB")
		("send-buffer-size", boost::program_options::value<typeof(send_buffer_size)>()->default_value((4)),
			"size of send buffer (in bytes), default is 4KB")
		("timeout", boost::program_options::value<typeof(timeout)>()->default_value(3000),
			"timeout for incoming conections (in ms), 0 stands for never timeout, defaults to 3000")
		("tcp-nodelay", boost::program_options::value<typeof(tcp_nodelay)>()->default_value(true),
			"enables tcp-nodelay feature or not, default is on")
		("memlock", boost::program_options::value<typeof(memlock)>()->default_value(false))
		("record-on-cache", boost::program_options::value<typeof(record_on_cache)>()->default_value(false))
		("stack-size", boost::program_options::value<typeof(stack_size)>()->default_value(staging::getRlimitCur(RLIMIT_STACK)))
		("cpuaffinity", boost::program_options::value<std::string>()->default_value(""))

		("io-threads", boost::program_options::value<typeof(io_threads)>()->default_value(1))
		("calculator-num", boost::program_options::value<typeof(calculator_num)>()->default_value(1))

		("message-max-size", boost::program_options::value<typeof(msg_max_size)>()->default_value(100))
		("max-connections", boost::program_options::value<typeof(max_connections)>()->default_value(10000))

		("reserve-features-count", boost::program_options::value<typeof(reserve_fnum)>()->default_value(200))

		("data-directory", boost::program_options::value<typeof(data_dir)>()->default_value("data"))
		("transformer-file", boost::program_options::value<typeof(transformer_file)>()->default_value(""))
		("classified-vector-file", boost::program_options::value<typeof(classified_vector_file)>()->default_value(""))
		("rocchio-optimized-file", boost::program_options::value<typeof(ropt_file)>()->default_value(""))

		("k", boost::program_options::value<typeof(k)>()->default_value(30))
		("rocchio-optimized-class-num", boost::program_options::value<typeof(ropt_cls_num)>()->default_value(10))
		("k-rate", boost::program_options::value<typeof(k)>()->default_value(0.1))
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
	logfile = options["log-file"].as<typeof(logfile)>();
	loglevel = options["log-level"].as<typeof(loglevel)>();
	reuse_address = options["reuse-address"].as<typeof(reuse_address)>();
	receive_buffer_size = options["receive-buffer-size"].as<typeof(receive_buffer_size)>() << 10;
	send_buffer_size = options["send-buffer-size"].as<typeof(send_buffer_size)>() << 10;
	timeout = options["timeout"].as<typeof(timeout)>();
	tcp_nodelay = options["tcp-nodelay"].as<typeof(tcp_nodelay)>();

	io_threads = options["io-threads"].as<typeof(io_threads)>();
	calculator_num = options["calculator-num"].as<typeof(calculator_num)>();

	max_connections = options["max-connections"].as<typeof(max_connections)>();
	msg_max_size = options["message-max-size"].as<typeof(msg_max_size)>() << 10;

	reserve_fnum = options["reserve-features-count"].as<typeof(reserve_fnum)>();

	data_dir = options["data-directory"].as<typeof(data_dir)>();
	transformer_file = options["transformer-file"].as<typeof(transformer_file)>();
	classified_vector_file = options["classified-vector-file"].as<typeof(classified_vector_file)>();
	ropt_file = options["rocchio-optimized-file"].as<typeof(ropt_file)>();

	k = options["k"].as<typeof(k)>();
	k_rate = options["k-rate"].as<typeof(k_rate)>();
	ropt_cls_num = options["rocchio-optimized-class-num"].as<typeof(ropt_cls_num)>();

	memlock = options["memlock"].as<typeof(memlock)>();
	record_on_cache = options["record-on-cache"].as<typeof(record_on_cache)>();
	if (memlock)
	{
#if CS_DEBUG
		int res = mlockall(MCL_CURRENT | MCL_FUTURE);
		assert(res == 0);
#else
		mlockall(MCL_CURRENT | MCL_FUTURE);
#endif
	}

	// "cpu-affinity" no longer useable since the threads-model changed.
	std::string cpumask = options["cpuaffinity"].as<std::string>();

	typedef boost::char_separator<char> Separator;
	typedef boost::tokenizer<Separator> Tokenizer;
	Separator sep("[],");
	Tokenizer tokens(cpumask, sep);

	if (cpumask.size())
	{
		uint cpunum = sysconf(_SC_NPROCESSORS_CONF);
		uint bits = cpunum * (calculator_num);
		if (cpumask.size() < bits)
		{
			cpumask.append(bits - cpumask.size(), '0');
		}
		cpuaffinity = boost::dynamic_bitset<>(cpumask);
	}

	CS_SAY("configs in <" << config_file << ">:" << std::endl
		_JEBE_OUT_CONFIG_PROPERTY(listen)
		_JEBE_OUT_CONFIG_PROPERTY(internal)
		_JEBE_OUT_CONFIG_PROPERTY(pidfile)
		_JEBE_OUT_CONFIG_PROPERTY(io_threads)
		_JEBE_OUT_CONFIG_PROPERTY(stack_size)
		_JEBE_OUT_CONFIG_PROPERTY(reuse_address)
		_JEBE_OUT_CONFIG_PROPERTY(receive_buffer_size)
		_JEBE_OUT_CONFIG_PROPERTY(send_buffer_size)
		_JEBE_OUT_CONFIG_PROPERTY(timeout)
		_JEBE_OUT_CONFIG_PROPERTY(tcp_nodelay)
		_JEBE_OUT_CONFIG_PROPERTY(msg_max_size)
		_JEBE_OUT_CONFIG_PROPERTY(max_connections)
		_JEBE_OUT_CONFIG_PROPERTY(calculator_num)
	);
}

} /* namespace rknn */
} /* namespace classify */
} /* namespace jebe */

#undef _JEBE_OUT_CONFIG_PROPERTY
