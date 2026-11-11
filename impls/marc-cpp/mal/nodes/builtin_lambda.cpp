module;

#include <vector>

module mal;

using namespace mal;

ConstNodePtr BuiltinLambda::eval(const ConstNodePtr &parent,
                                 const std::vector<ConstNodePtr> &args) const {
  return this->func(parent, args);
}

std::string BuiltinLambda::print() const { return this->name; }