
#include "portal.hpp"
#include "config.hpp"

int main(int argc, char* argv[])
{
	using namespace jebe::cws;

	Config::getInstance()->init(argc, argv);
	Portal portal;
    portal.run();
    
    return 0;
}
