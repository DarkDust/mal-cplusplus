module;

#include <string>

module mal;

using namespace mal;

Number::Number(const std::string &str) : num(std::stoi(str)) {}

Number::Number(const int num) : num(num) {}

std::string Number::print() const { return std::to_string(this->num); }

int Number::get_number() const { return this->num; }