
#include "request_handler.hpp"

namespace jebe {
namespace cws {

const Filter* RequestHandler::filter;
std::size_t RequestHandler::chunkSize;

const RequestHandler::ActionList RequestHandler::actionList= {{
	*reinterpret_cast<const uint64_t*>("split   "),
	*reinterpret_cast<const uint64_t*>("count   "),
	*reinterpret_cast<const uint64_t*>("compare "),
	*reinterpret_cast<const uint64_t*>("marve   "),
}};

}
}
