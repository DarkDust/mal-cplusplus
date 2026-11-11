module;

#include <string>

module mal;

using namespace mal;

Number::Number(const ConstLocationRangePtr &location, const std::string &str)
    : Node(location), num(std::stoi(str)) {}

Number::Number(const ConstLocationRangePtr &location, const int num)
    : Node(location), num(num) {}

bool Number::equals(const NonNullSharedPtr<const Node> &maybe_other) const {
  auto other = maybe_other.cast<const Number>();
  if (!other) {
    return false;
  }

  return (*other)->num == this->num;
};

std::string Number::print() const { return std::to_string(this->num); }

int Number::get_number() const { return this->num; }