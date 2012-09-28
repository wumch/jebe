
#include "master.hpp"

int main(int argc, char* argv[])
{
	using namespace jebe::policy;
	(new Master(2, "<cross-domain-policy><allow-access-from domain=\"*\" to-ports=\"*\"/></cross-domain-policy>"))->run();
    return 0;
}
