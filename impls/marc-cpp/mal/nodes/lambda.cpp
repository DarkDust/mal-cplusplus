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

  for (decltype(binds)::size_type i = 0; i < num_binds; ++i) {
    const auto &key_node = this->binds.at(i).cast<const Symbol>();
    if (!key_node) {
      throw RuntimeError(this->binds.at(i)->get_location(), "not a symbol");
    }

    const auto &key = (*key_node)->get_symbol();
    if (key == "&") {
      // Clojure-style variadic function argument. The next symbol is used to
      // grab all remaining arguments as a list.
      if (i + 1 == num_binds) {
        throw RuntimeError((*key_node)->get_location(),
                           "missing variadic name");
      }

      const auto &variadic_node = this->binds.at(i + 1).cast<const Symbol>();
      if (!variadic_node) {
        throw RuntimeError(this->binds.at(i + 1)->get_location(),
                           "not a symbol");
      }

      const auto start =
          args.begin() + static_cast<decltype(binds)::difference_type>(
                             std::min(i, args.size()));
      const auto remainder = std::vector(start, args.end());
      const auto remainder_list = make_nonnull_shared<List>(
          LocationRange::create_virtual(), List::Kind::List, remainder);
      sub_env->set((*variadic_node)->get_symbol(), remainder_list);
      break;
    }

    if (i < num_args) {
      const auto &arg = args.at(i);
      sub_env->set(key, arg);

    } else {
      sub_env->set(key,
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
