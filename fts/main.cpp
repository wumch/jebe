
#include "config.hpp"
#include "master.hpp"

int main(int argc, char* argv[])
{
	using namespace jebe::fts;

	Config::getInst()->init(argc, argv);
	(new Master)->run();

    return 0;
}
