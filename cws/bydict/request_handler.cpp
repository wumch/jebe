
#include "request_handler.hpp"

namespace jebe {
namespace cws {

Filter* RequestHandler::filter;
std::size_t RequestHandler::chunkSize;

const RequestHandler::ActionList RequestHandler::actionList= {{
	*reinterpret_cast<const uint64_t*>("split   "),
	*reinterpret_cast<const uint64_t*>("count   "),
	*reinterpret_cast<const uint64_t*>("compare "),
}};

}
}
