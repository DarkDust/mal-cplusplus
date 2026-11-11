module;

#include <string>

module mal;

using namespace mal;

Future::Future(const ConstLocationRangePtr &location, std::string prefix,
               std::string suffix, const ConstNodePtr &contained)
    : Node(location), prefix(std::move(prefix)), suffix(std::move(suffix)),
      contained(contained) {}

bool Future::equals(const NonNullSharedPtr<const Node> &maybe_other) const {
  auto other = maybe_other.cast<const Future>();
  if (!other) {
    return false;
  }

  return (*other)->contained->equals(this->contained);
};

std::string Future::print() const {
  return this->prefix + this->contained->print() + this->suffix;
}