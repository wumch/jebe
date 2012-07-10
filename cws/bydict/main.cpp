
#include "portal.hpp"

int main(int argc, char* argv[])
{
    const char *conf;
    switch (argc)
    {
        case 2:
            conf = argv[1];
            break;
        case 3:
            conf = argv[2];
            break;
        default:
#if HAS_LUAPLUS
            std::cerr << "Usage: " << argv[0] << " -f path-to-config-file\n";
            return 1;
#endif
            break;
    }

    using namespace jebe::cws;
    Portal portal(conf);
    portal.run();
    
    return 0;
}
