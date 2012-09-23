
#include "preprocess.hpp"

int main(int argc, char* argv[])
{
	using namespace jebe::cws;

	Config::getInst()->init(argc, argv);
	Preprocess().run();

    return 0;
}
