module;

#include <string>

module mal;

using namespace mal;

Number::Number(const std::string &str) {
  // This can throw!
  this->num = std::stoi(str);
}

std::string Number::print() const { return std::to_string(this->num); }