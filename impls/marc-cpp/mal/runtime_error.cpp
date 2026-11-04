module;

#include <format>
#include <stdexcept>
#include <string>

module mal;

using namespace mal;

RuntimeError::RuntimeError(const ConstLocationRangePtr &location,
                           const std::string &message)
    : std::runtime_error{message}, location(location) {}

std::string RuntimeError::print() const {
  return std::format("{}: {}", this->location->print(), this->what());
}