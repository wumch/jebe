
#include "config.hpp"
#include "aside.hpp"
#include "svd.hpp"

#undef __FUNCT__
#define __FUNCT__ "main"
int main(int argc, char* argv[])
{
	using namespace jebe::dsvd::svd;
	Config::getInst()->init(argc, argv);
	Aside::init();
	DSVD* svd = new DSVD;
	svd->run();
}
