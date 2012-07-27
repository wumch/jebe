
#include "config.hpp"
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
extern "C" {
#include <unistd.h>
#include <sys/mman.h>
#include <sched.h>
}

namespace jebe {
namespace cws {

void Config::init(int argc, char* argv[])
{
	boost::program_options::options_description cmdDesc("allowed options");
	cmdDesc.add_options()
			("help", "show this help and exit.")
			("config", boost::program_options::value<std::string>()->default_value("etc/tokenizer.conf"),
					"config file, defaults to etc/tokenizer.conf.")
	;

	boost::filesystem::path program_path = argv[0];
	program_name = program_path.filename();

	boost::program_options::variables_map cmdOptions;
	try
	{
		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, cmdDesc, boost::program_options::command_line_style::unix_style), cmdOptions);
	}
	catch (std::exception& e)
	{
		CS_DIE(e.what() << "\n" << cmdDesc);
	}
	boost::program_options::notify(cmdOptions);

	if (cmdOptions.count("help"))
	{
		std::cout << cmdDesc << std::endl;
	}
	else if (cmdOptions.count("config"))
	{
		initDesc();
		load(cmdOptions["config"].as<std::string>());
	}
}

void Config::initDesc()
{
	desc.add_options()
			("host", boost::program_options::value<typeof(host)>()->default_value("0.0.0.0"),
					"host, defaults to 0.0.0.0")
			("port", boost::program_options::value<typeof(port)>()->default_value(10087),
					"port, defaults to 10087")
			("pid-file", boost::program_options::value<typeof(pidfile)>()->default_value("/var/run/tokenizer.pid"),
					"pid file, defaults to /var/run/tokenizer.pid")
			("patten-file", boost::program_options::value<typeof(patten_file)>()->default_value("etc/words.txt"),
					"pattens file, defaults to etc/words.txt")
			("reuse-address", boost::program_options::value<typeof(reuse_address)>()->default_value(true),
					"whether reuse-address on startup or not")
			("receive-buffer-size", boost::program_options::value<typeof(receive_buffer_size)>()->default_value((4 << 10)),
					"siz eof receive buffer (in bytes)")
			("send-buffer-size", boost::program_options::value<typeof(send_buffer_size)>()->default_value((4 << 10)),
					"size of send buffer (in bytes)")
			("timeout", boost::program_options::value<typeof(timeout)>()->default_value(3000),
					"timeout for incoming conections (in ms), 0 stands for never timeout, defaults to 3000")
			("tcp-nodelay", boost::program_options::value<typeof(tcp_nodelay)>()->default_value(true),
					"enables tcp-nodelay feature or not, defaults is on")
			("memlock", boost::program_options::value<typeof(memlock)>()->default_value(false))
			("cpuaffinity", boost::program_options::value<std::string>()->default_value(""))
			("worker-count", boost::program_options::value<typeof(worker_count)>()->default_value(sysconf(_SC_NPROCESSORS_CONF)))
			("header-max-size", boost::program_options::value<typeof(header_max_size)>()->default_value(1024))
			("body-max-size", boost::program_options::value<typeof(body_max_size)>()->default_value(64000))
			("max-connections", boost::program_options::value<typeof(max_connections)>()->default_value(10000))
			("max-write-times", boost::program_options::value<typeof(max_write_times)>()->default_value(100))
			("request-max-size", boost::program_options::value<typeof(request_max_size)>()->default_value(64000 + 1024))
			;
}

void Config::load(const std::string& config_file)
{
	try
	{
		boost::program_options::store(boost::program_options::parse_config_file<char>(config_file.c_str(), desc), options);
	}
	catch (std::exception& e)
	{
		CS_DIE(L"faild on read/parse config-file: " << config_file.c_str() << L"\n" << e.what());
	}
	boost::program_options::notify(options);

	host = options["host"].as<typeof(host)>();
	port = options["port"].as<typeof(port)>();
	pidfile = options["pid-file"].as<typeof(pidfile)>();
	patten_file = options["patten-file"].as<typeof(patten_file)>();
	reuse_address = options["reuse-address"].as<typeof(reuse_address)>();
	receive_buffer_size = options["receive-buffer-size"].as<typeof(receive_buffer_size)>();
	send_buffer_size = options["send-buffer-size"].as<typeof(send_buffer_size)>();
	timeout = options["timeout"].as<typeof(timeout)>();
	tcp_nodelay = options["tcp-nodelay"].as<typeof(tcp_nodelay)>();
	worker_count = options["worker-count"].as<typeof(worker_count)>();
	header_max_size = options["header-max-size"].as<typeof(header_max_size)>();
	body_max_size = options["body-max-size"].as<typeof(body_max_size)>();
	max_connections = options["max-connections"].as<typeof(max_connections)>();
	max_write_times = options["max-write-times"].as<typeof(max_write_times)>();
	request_max_size = options["request-max-size"].as<typeof(request_max_size)>();

	memlock = options["memlock"].as<typeof(memlock)>();
	if (memlock)
	{
		assert(!mlockall(MCL_CURRENT | MCL_FUTURE));
	}

	// `cpu-affinity` no longer useable since the threads-model changed.
	std::string cpumask = options["cpuaffinity"].as<std::string>();

	typedef boost::char_separator<char> Separator;
	typedef boost::tokenizer<Separator> Tokenizer;
	Separator sep("[],");
	Tokenizer tokens(cpumask, sep);

	if (cpumask.size())
	{
		uint cpunum = sysconf(_SC_NPROCESSORS_CONF);
		uint bits = cpunum * worker_count;
		if (cpumask.size() < bits)
		{
			cpumask.append(bits - cpumask.size(), '0');
		}
		cpuaffinity = boost::dynamic_bitset<>(cpumask);
	}

	CS_SAY("configs in <" << config_file << ">:" << std::endl
		<< host << std::endl
		<< port << std::endl
		<< pidfile << std::endl
		<< patten_file << std::endl
		<< reuse_address << std::endl
		<< receive_buffer_size << std::endl
		<< send_buffer_size << std::endl
		<< timeout << std::endl
		<< tcp_nodelay << std::endl
		<< worker_count << std::endl
		<< header_max_size << std::endl
		<< body_max_size << std::endl
		<< max_connections << std::endl
		<< max_write_times << std::endl
		<< request_max_size << std::endl
	);
}

}
}
