module;

#include <string>

module mal;

using namespace mal;

Bool::Bool(const ConstLocationRangePtr &range, bool value)
    : Node(range), value(value) {};

bool Bool::is_trueish() const { return this->value; }

std::string Bool::print() const { return this->value ? "true" : "false"; }
