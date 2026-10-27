module;

#include <string>

module mal;

using namespace mal;

Symbol::Symbol(const std::string &str) { this->sym = str; }

std::string Symbol::print() const { return this->sym; }