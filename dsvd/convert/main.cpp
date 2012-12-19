
#include "convert.hpp"

#undef __FUNCT__
#define __FUNCT__ "main"
int main(int argc, char* argv[])
{
	using namespace jebe::dsvd::convert;
	Converter* c = new Converter(argc, argv);
	c->run();
}
