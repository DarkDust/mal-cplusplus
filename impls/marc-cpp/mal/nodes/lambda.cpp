module;

#include <vector>

module mal;

using namespace mal;

Lambda::Lambda(const ConstLocationRangePtr &range,
               std::vector<ConstNodePtr> binds, const ConstNodePtr &expr,
               const EnvironmentPtr &env)
    : Node(range), binds(std::move(binds)), expr(expr), env(env) {};

ConstNodePtr Lambda::eval(const std::vector<ConstNodePtr> &args,
                          const Evaluator &evaluator) const {
  auto sub_env = make_nonnull_shared<Environment>(this->env);
  const auto num_binds = this->binds.size();
  const auto num_args = args.size();

  for (auto i = 0; i < num_binds; ++i) {
    const auto &key = this->binds.at(i).cast<const Symbol>();
    if (!key) {
      throw RuntimeError(this->binds.at(i)->get_location(), "not a symbol");
    }

    if (i < num_args) {
      const auto &arg = args.at(i);
      sub_env->set((*key)->get_symbol(), arg);

    } else {
      sub_env->set((*key)->get_symbol(),
                   make_nonnull_shared<Nil>(LocationRange::create_virtual()));
    }
  }

  return evaluator.eval(this->expr, sub_env);
};

bool Lambda::equals(const NonNullSharedPtr<const Node> &maybed_other) const {
  auto other = maybed_other.cast<const Lambda>();
  if (other) {
    return (*other).shared().get() == this;
  }
  return false;
};

std::string Lambda::print() const { return "#<function>"; }
