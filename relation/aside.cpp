
#include "aside.hpp"
#include "calculater.hpp"

namespace jebe {
namespace rel {

WordMap Aside::wordmap;

const Config* const Aside::config = Config::getInstance();
Calculater* const Aside::caler = new Calculater;

docnum_t Aside::curDocNum = 0;

} /* namespace rel */
} /* namespace jebe */
