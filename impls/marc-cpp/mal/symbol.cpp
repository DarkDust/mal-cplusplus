module;

#include <string>

module mal;

using namespace mal;

Symbol::Symbol(std::string str) : sym(std::move(str)) {}

std::string Symbol::print() const { return this->sym; }

const std::string &Symbol::get_symbol() const { return this->sym; }