module;

#include <string>

module mal;

using namespace mal;

Symbol::Symbol(const ConstLocationRangePtr &location, std::string str)
    : Node(location), sym(std::move(str)) {}

bool Symbol::equals(const NonNullSharedPtr<const Node> &maybe_other) const {
  auto other = maybe_other.cast<const Symbol>();
  if (!other) {
    return false;
  }

  return (*other)->sym == this->sym;
};

std::string Symbol::print() const { return this->sym; }

const std::string &Symbol::get_symbol() const { return this->sym; }