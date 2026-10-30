module;

#include <string>

module mal;

using namespace mal;

Future::Future(std::string prefix, std::string suffix, ConstNodePtr contained)
    : prefix(std::move(prefix)), suffix(std::move(suffix)),
      contained(std::move(contained)) {}

std::string Future::print() const {
  return this->prefix + this->contained->print() + this->suffix;
}