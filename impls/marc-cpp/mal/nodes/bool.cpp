module;

#include <string>

module mal;

using namespace mal;

Bool::Bool(const ConstLocationRangePtr &range, bool value)
    : Node(range), value(value) {};

bool Bool::is_trueish() const { return this->value; }

bool Bool::equals(const NonNullSharedPtr<const Node> &maybe_other) const {
  auto other = maybe_other.cast<const Bool>();
  if (!other) {
    return false;
  }

  return (*other)->value == this->value;
};

std::string Bool::print() const { return this->value ? "true" : "false"; }
