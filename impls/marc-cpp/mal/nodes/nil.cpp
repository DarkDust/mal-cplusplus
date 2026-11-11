module;

#include <string>

module mal;

using namespace mal;

Nil::Nil(const ConstLocationRangePtr &range) : Node(range) {};

bool Nil::equals(const NonNullSharedPtr<const Node> &maybe_other) const {
  return maybe_other.cast<const Nil>().has_value();
};

bool Nil::is_trueish() const { return false; }

std::string Nil::print() const { return "nil"; }
