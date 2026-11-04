module;

#include <format>
#include <stdexcept>
#include <string>

module mal;

using namespace mal;

ParseError::ParseError(const ConstLocationRangePtr &location,
                       const std::string &message)
    : std::runtime_error{message}, location(location) {}

std::string ParseError::print() const {
  return std::format("{}: {}", this->location->print(), this->what());
}