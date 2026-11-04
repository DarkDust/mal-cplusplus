module;

#include <string>

module mal;

using namespace mal;

Future::Future(const ConstLocationRangePtr &location, std::string prefix,
               std::string suffix, const ConstNodePtr &contained)
    : Node(location), prefix(std::move(prefix)), suffix(std::move(suffix)),
      contained(contained) {}

std::string Future::print() const {
  return this->prefix + this->contained->print() + this->suffix;
}