
#include "aside.hpp"
#include "master.hpp"

int main(int argc, char* argv[])
{
	using namespace jebe::cluster::ets;

	Aside::init(argc, argv);
	(new Master)->run();

    return 0;
}
