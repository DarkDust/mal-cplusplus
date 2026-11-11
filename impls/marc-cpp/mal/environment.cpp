module;

#include <functional>
#include <optional>
#include <vector>

module mal;

using namespace mal;

using ArithmeticOperation =
    std::function<int(int, int, const ConstNodePtr &, const ConstNodePtr &)>;

static BuiltinLambda::Func wrap_arithmetic(const ArithmeticOperation &op) {
  return [op](const ConstNodePtr &parent,
              const std::vector<ConstNodePtr> &args) {
    if (args.size() != 2) {
      throw RuntimeError(parent->get_location(), "invalid number of arguments");
    }

    auto a = args.at(0).cast<const Number>();
    auto b = args.at(1).cast<const Number>();
    if (!a) {
      throw RuntimeError(args.at(0)->get_location(),
                         "argument is not a number");
    }
    if (!b) {
      throw RuntimeError(args.at(1)->get_location(),
                         "argument is not a number");
    }

    int result = op((*a)->get_number(), (*b)->get_number(), *a, *b);
    return make_nonnull_shared<Number>(LocationRange::create_virtual(), result);
  };
}

class BuiltinLambdaArithmetic : public BuiltinLambda {
public:
  using Operation =
      std::function<int(int, int, const ConstNodePtr &, const ConstNodePtr &)>;

  BuiltinLambdaArithmetic(std::string name, const Operation &op)
      : BuiltinLambda(name, wrap_arithmetic(op)) {}
};

Environment::Environment(std::optional<NonNullSharedPtr<Environment>> parent)
    : parent(std::move(parent)), map{} {}

std::optional<ConstNodePtr> Environment::get(const std::string &key) const {
  auto r = this->map.find(key);
  if (r != this->map.end()) {
    return (*r).second;
  } else if (this->parent) {
    return (*this->parent)->get(key);
  } else {
    return std::nullopt;
  }
}

void Environment::set(const std::string &key, ConstNodePtr val) {
  this->map.insert_or_assign(key, val);
}

void Environment::remove(const std::string &key) { this->map.erase(key); }

void Environment::populate_with_default() {
  auto add = [this](const std::string &name, BuiltinLambda::Func func) {
    auto lambda = make_nonnull_shared<BuiltinLambda>(name, std::move(func));
    this->set(name, lambda);
  };

  auto add_arithmetic = [this](const std::string &name,
                               BuiltinLambdaArithmetic::Operation op) {
    auto lambda =
        make_nonnull_shared<BuiltinLambdaArithmetic>(name, std::move(op));
    this->set(name, lambda);
  };

  add_arithmetic("+", [](int a, int b, const ConstNodePtr &node_a,
                         const ConstNodePtr &node_b) { return a + b; });
  add_arithmetic("-", [](int a, int b, const ConstNodePtr &node_a,
                         const ConstNodePtr &node_b) { return a - b; });
  add_arithmetic("*", [](int a, int b, const ConstNodePtr &node_a,
                         const ConstNodePtr &node_b) { return a * b; });
  add_arithmetic("/", [](int a, int b, const ConstNodePtr &node_a,
                         const ConstNodePtr &node_b) {
    if (b == 0) {
      throw RuntimeError(node_b->get_location(), "division by zero");
    }

    return a / b;
  });

  add("list",
      [](const ConstNodePtr &parent, const std::vector<ConstNodePtr> &args) {
        return make_nonnull_shared<const List>(LocationRange::create_virtual(),
                                               List::Kind::List, args);
      });

  add("list?", [](const ConstNodePtr &parent,
                  const std::vector<ConstNodePtr> &args) {
    if (args.size() != 1) {
      throw RuntimeError(parent->get_location(), "wrong number of arguments");
    }

    auto maybe_list = args.at(0).cast<const List>();
    if (maybe_list) {
      switch ((*maybe_list)->get_kind()) {
      case List::Kind::List:
        return make_nonnull_shared<const Bool>(parent->get_location(), true);

      case List::Kind::Vector:
        return make_nonnull_shared<const Bool>(parent->get_location(), false);
      }
    } else {
      return make_nonnull_shared<const Bool>(parent->get_location(), false);
    }
  });
}
