
#include "portal.hpp"
#include "config.hpp"

int main(int argc, char* argv[])
{
	using namespace jebe::cwshttp;

	Config::getInst()->init(argc, argv);
	(new Portal)->run();
    
    return 0;
}
