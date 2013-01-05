
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
#include <boost/logic/tribool_io.hpp>
extern "C" {
#include <sched.h>
}
#include "sys.hpp"
#include "net.hpp"

#define _JEBE_OUT_CONFIG_PROPERTY(property)		<< CS_OC_RED(#property) << ":\t\t" << CS_OC_GREEN(property) << std::endl

namespace jebe {
namespace dsvd {
namespace svd {

void Config::init(int argc_, char* argv_[])
{
	argc = argc_;
	argv = argv_;
	argv_first = argv[0];

	boost::filesystem::path program_path = argv[0];
#if BOOST_VERSION > 104200
	program_name = program_path.filename().string();
#else
	program_name = program_path.filename();
#endif

	boost::filesystem::path default_config_file("etc/" + program_name + ".conf");

	boost::filesystem::path default_pidfile(std::string("/var/run/") + program_name + ".pid");
	boost::filesystem::path default_logfile(std::string("/var/log/") + program_name + ".log");

	desc.add_options()
		("help,h", "show this help and exit.")
		("config,c", boost::program_options::value<typeof(config_file)>()->default_value(default_config_file),
			("config file, defaults to " + default_config_file.string() + ".").c_str())
		("no-config-file", boost::program_options::bool_switch()->default_value(false),
			"force do not load options from config file, defaults to false.")

		("pid-file", boost::program_options::value<typeof(pidfile)>()->default_value(default_pidfile),
			(std::string("pid file, defaults to ") + default_pidfile.string()).c_str())

		("log-file", boost::program_options::value<typeof(logfile)>()->default_value(default_logfile),
			(std::string("log file, defaults to ") + default_logfile.string() + ".").c_str())
		("log-level", boost::program_options::value<typeof(loglevel)>()->default_value(0),
			"0,1 or 2. the higher, the more verbose, and 0 stands for disable log.")

		("svd_nsv", boost::program_options::value<typeof(nsv)>()->default_value(0),
			"num of singular-values being expected. default: 0, stands for all.")
		("svd_ncv", boost::program_options::value<typeof(ncv)>()->default_value(0),
			"num of columns should be used to calculate singular-values. default: 0, stands for all.")

		("sort-asc", boost::program_options::bool_switch()->default_value(false),
			"whether sort singular-value ascending or not (descending). default: no (i.e. descending).")

		("daemon,d", boost::program_options::bool_switch()->default_value(false),
			"whether run as a daemon or not. default: no.")
		("memlock", boost::program_options::bool_switch()->default_value(false),
			"as u know.")
		("cpuaffinity", boost::program_options::value<std::string>()->default_value(""),
			"as u know.")

		("matrix-file,m", boost::program_options::value<typeof(matfile)>()->default_value(""),
			"input matrix-file, in SPESc binary format.")

		("output-dir,o", boost::program_options::value<typeof(output_dir)>()->default_value("./data"),
			"directory of output matrix files.")
		("matrix-file-extension", boost::program_options::value<typeof(matfile_ext)>()->default_value(".mtx"),
			"extension of output matrix files.")
		("solution-file-extension", boost::program_options::value<typeof(solution_file_ext)>()->default_value(".dat"),
			"extension of solution file (for binary file).")
		("text-file-extension", boost::program_options::value<typeof(solution_file_ext)>()->default_value(".txt"),
			"extension of plain text file.")

		("store-USV", boost::program_options::value<typeof(store_USV)>()->default_value(nonspecified),
			"whether store USV or not. default: auto guess from other options such as \"matrix-file-U\".")
		("matrix-file-U", boost::program_options::value<typeof(outfile_U)>()->default_value(""),
			"path of output matrix file -- U.")
		("matrix-file-S", boost::program_options::value<typeof(outfile_S)>()->default_value(""),
			"path of output matrix file -- S.")
		("matrix-file-Vt", boost::program_options::value<typeof(outfile_Vt)>()->default_value(""),
			"path of output matrix file -- Vt. (NOTE: whether it's transposed or not depends on another option which called \"transpose-v\").")
		("transpose-Vt", boost::program_options::bool_switch()->default_value(false),
			"whether transpose Vt to real V or not. default: no, indicates that Vt -- which already transposed V will be kept.")

		("store-solution", boost::program_options::value<typeof(store_solution)>()->default_value(nonspecified),
			"whether store solution or not. default: auto guess from \"solution-file\".")
		("solution-file", boost::program_options::value<typeof(outfile_solution)>()->default_value(""),
			"path of output solution file (in binary format).")
		("solution-file-text", boost::program_options::value<typeof(outfile_solution_text)>()->default_value(""),
			"path of output solution file (in plain text format).")

		("store-product", boost::program_options::value<typeof(store_product)>()->default_value(nonspecified),
			"whether store the product of U*S or not. default: auto guess from \"matrix-file-SvUt\" or \"matrix-file-feature-space\".")
		("matrix-file-SvUt", boost::program_options::value<typeof(outfile_SvUt)>()->default_value(""),
			"path of output solution file.")
		("matrix-file-feature-space", boost::program_options::value<typeof(outfile_feature_space)>()->default_value(""),
			"path of output matrix file -- Vk (= SvUt * A), the feature space.")
	;

	try
	{
		boost::program_options::command_line_parser parser(argc, argv);
		parser.options(desc).allow_unregistered().style(cmd_style);
		boost::program_options::store(parser.run(), options);
	}
	catch (const std::exception& e)
	{
		CS_DIE(CS_OC_RED(e.what()) << CS_LINESEP << desc);
	}
	boost::program_options::notify(options);

	if (options.count("help"))
	{
		std::cout << desc << std::endl;
		std::exit(0);
	}
	else
	{
		bool no_config_file = options["no-config-file"].as<bool>();
		if (!no_config_file)
		{
			config_file = options["config"].as<typeof(config_file)>();
			load();
		}
	}
}

void Config::load()
{
	try
	{
		boost::program_options::store(boost::program_options::parse_config_file<char>(config_file.c_str(), desc), options);
	}
	catch (const std::exception& e)
	{
		CS_DIE("faild on read/parse config-file: " << config_file.c_str() << "\n" << CS_OC_RED(e.what()));
	}
	boost::program_options::notify(options);
	pidfile = options["pid-file"].as<typeof(pidfile)>();
	logfile = options["log-file"].as<typeof(logfile)>();
	loglevel = options["log-level"].as<typeof(loglevel)>();

	nsv = options["svd_nsv"].as<typeof(nsv)>();
	ncv = options["svd_ncv"].as<typeof(nsv)>();
	asc = options["sort-asc"].as<typeof(asc)>();

	matfile = options["matrix-file"].as<typeof(matfile)>();

	output_dir = options["output-dir"].as<typeof(output_dir)>();
	matfile_ext = options["matrix-file-extension"].as<typeof(matfile_ext)>();
	solution_file_ext = options["solution-file-extension"].as<typeof(solution_file_ext)>();

	store_USV = options["store-USV"].as<typeof(store_USV)>();
	store_solution = options["store-solution"].as<typeof(store_solution)>();
	store_product = options["store-product"].as<typeof(store_product)>();

	outfile_U = options["matrix-file-U"].as<typeof(outfile_U)>();
	outfile_S = options["matrix-file-S"].as<typeof(outfile_S)>();
	outfile_Vt = options["matrix-file-Vt"].as<typeof(outfile_Vt)>();

	outfile_solution = options["solution-file"].as<typeof(outfile_solution)>();
	outfile_solution_text = options["solution-file-text"].as<typeof(outfile_solution_text)>();

	outfile_SvUt = options["matrix-file-SvUt"].as<typeof(outfile_SvUt)>();
	outfile_feature_space = options["matrix-file-feature-space"].as<typeof(outfile_feature_space)>();

	memlock = options["memlock"].as<typeof(memlock)>();
	if (memlock)
	{
#if CS_DEBUG
		assert(!mlockall(MCL_CURRENT | MCL_FUTURE));
#else
		mlockall(MCL_CURRENT | MCL_FUTURE);
#endif
	}

	// `cpu-affinity` no longer useable since the threads-model changed.
	std::string cpumask = options["cpuaffinity"].as<std::string>();

	typedef boost::char_separator<char> Separator;
	typedef boost::tokenizer<Separator> Tokenizer;
	Separator sep("[],");
	Tokenizer tokens(cpumask, sep);

	if (cpumask.size())
	{
		uint cpunum = staging::getCpuNum();
		uint bits = cpunum;
		if (cpumask.size() < bits)
		{
			cpumask.append(bits - cpumask.size(), '0');
		}
		cpuaffinity = boost::dynamic_bitset<>(cpumask);
	}

	solve_files();

	CS_SAY("configs in [" << config_file << "]:" << std::endl
		_JEBE_OUT_CONFIG_PROPERTY(program_name)
		_JEBE_OUT_CONFIG_PROPERTY(as_daemon)
		_JEBE_OUT_CONFIG_PROPERTY(pidfile)
		_JEBE_OUT_CONFIG_PROPERTY(logfile)
		_JEBE_OUT_CONFIG_PROPERTY(loglevel)
		_JEBE_OUT_CONFIG_PROPERTY(memlock)
		_JEBE_OUT_CONFIG_PROPERTY(nsv)
		_JEBE_OUT_CONFIG_PROPERTY(ncv)
		_JEBE_OUT_CONFIG_PROPERTY(asc)
		_JEBE_OUT_CONFIG_PROPERTY(matfile)

		_JEBE_OUT_CONFIG_PROPERTY(output_dir)

		_JEBE_OUT_CONFIG_PROPERTY(store_USV)
		_JEBE_OUT_CONFIG_PROPERTY(outfile_U)
		_JEBE_OUT_CONFIG_PROPERTY(outfile_S)
		_JEBE_OUT_CONFIG_PROPERTY(outfile_Vt)

		_JEBE_OUT_CONFIG_PROPERTY(store_solution)
		_JEBE_OUT_CONFIG_PROPERTY(outfile_solution)
		_JEBE_OUT_CONFIG_PROPERTY(outfile_solution_text)

		_JEBE_OUT_CONFIG_PROPERTY(store_product)
		_JEBE_OUT_CONFIG_PROPERTY(outfile_SvUt)
		_JEBE_OUT_CONFIG_PROPERTY(outfile_feature_space)
	);
}

void Config::solve_files()
{
	if (nonspecified(store_USV))
	{
		store_USV = !outfile_U.empty()
			|| !outfile_S.empty()
			|| !outfile_Vt.empty();
	}
	// solve "outfile-solution".
	if (nonspecified(store_solution))
	{
		store_solution = !outfile_solution.empty();
	}
	// solve "calcu-product" before overwrite any options.
	if (nonspecified(store_product))
	{
		store_product = !(outfile_feature_space.empty() && outfile_SvUt.empty());
	}

	if (!output_dir.empty())
	{
		// U * S * Vt
		if (outfile_U.empty())
		{
			outfile_U = output_dir / "U";
		}
		if (outfile_S.empty())
		{
			outfile_S = output_dir / "S";
		}
		if (outfile_Vt.empty())
		{
			outfile_Vt = output_dir / "Vt";
		}

		// solution
		if (outfile_solution.empty())
		{
			outfile_solution = output_dir / "solution";
		}
		if (outfile_solution_text.empty())
		{
			outfile_solution_text = outfile_solution;
		}

		// Sv * Ut = E / (U * S)
		if (outfile_SvUt.empty())
		{
			outfile_SvUt = output_dir / "SvUt";
		}

		// Vk, the feature space.
		if (outfile_feature_space.empty())
		{
			outfile_feature_space = output_dir / "Vk";
		}
	}

	if (!outfile_U.has_extension())
	{
		outfile_U.replace_extension(matfile_ext);
	}
	if (!outfile_S.has_extension())
	{
		outfile_S.replace_extension(matfile_ext);
	}
	if (!outfile_Vt.has_extension())
	{
		outfile_Vt.replace_extension(matfile_ext);
	}
	if (!outfile_solution.has_extension())
	{
		outfile_solution.replace_extension(solution_file_ext);
	}
	if (!outfile_solution_text.has_extension())
	{
		outfile_solution_text.replace_extension(text_file_ext);
	}
	if (!outfile_SvUt.has_extension())
	{
		outfile_SvUt.replace_extension(matfile_ext);
	}
	if (!outfile_feature_space.has_extension())
	{
		outfile_feature_space.replace_extension(matfile_ext);
	}
}

} /* namespace svd */
} /* namespace dsvd */
} /* namespace jebe */

#undef _JEBE_OUT_CONFIG_PROPERTY
