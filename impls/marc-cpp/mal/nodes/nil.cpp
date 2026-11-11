module;

#include <string>

module mal;

using namespace mal;

Nil::Nil(const ConstLocationRangePtr &range) : Node(range) {};

bool Nil::is_trueish() const { return false; }

std::string Nil::print() const { return "nil"; }
