module;

#include <string>

module mal;

using namespace mal;

Symbol::Symbol(const ConstLocationRangePtr &location, std::string str)
    : Node(location), sym(std::move(str)) {}

std::string Symbol::print() const { return this->sym; }

const std::string &Symbol::get_symbol() const { return this->sym; }