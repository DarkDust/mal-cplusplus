module;

#include <string>

module mal;

using namespace mal;

Number::Number(const ConstLocationRangePtr &location, const std::string &str)
    : Node(location), num(std::stoi(str)) {}

Number::Number(const ConstLocationRangePtr &location, const int num)
    : Node(location), num(num) {}

std::string Number::print() const { return std::to_string(this->num); }

int Number::get_number() const { return this->num; }