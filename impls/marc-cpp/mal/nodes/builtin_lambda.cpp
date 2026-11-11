module;

#include <vector>

module mal;

using namespace mal;

ConstNodePtr BuiltinLambda::eval(const ConstNodePtr &parent,
                                 const std::vector<ConstNodePtr> &args) const {
  return this->func(parent, args);
}

bool BuiltinLambda::equals(
    const NonNullSharedPtr<const Node> &maybe_other) const {
  // Lambdas are never equal, except if it's the same instance.
  return this == maybe_other.shared().get();
};

std::string BuiltinLambda::print() const { return this->name; }