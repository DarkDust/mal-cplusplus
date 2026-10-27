module;

#include <string>

module mal;

using namespace mal;

Future::Future(const std::string &prefix, const std::string &suffix,
               std::shared_ptr<Node> contained)
    : prefix(prefix), suffix(suffix), contained(contained) {}

std::string Future::print() const {
  return this->prefix + this->contained->print() + this->suffix;
}