module;

#include <functional>
#include <iostream>
#include <optional>
#include <vector>

module mal;

using namespace mal;

using ArithmeticOperation =
    std::function<int(int, int, const ConstNodePtr &, const ConstNodePtr &)>;

using ComparisonOperation =
    std::function<bool(int, int, const ConstNodePtr &, const ConstNodePtr &)>;

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
    return make_nonnull_shared<Number>(parent->get_location(), result);
  };
}

static BuiltinLambda::Func wrap_comparison(const ComparisonOperation &op) {
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
    return make_nonnull_shared<Bool>(parent->get_location(), result);
  };
}

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
                               const ArithmeticOperation &op) {
    auto lambda = make_nonnull_shared<BuiltinLambda>(name, wrap_arithmetic(op));
    this->set(name, lambda);
  };

  auto add_comparison = [this](const std::string &name,
                               const ComparisonOperation &op) {
    auto lambda = make_nonnull_shared<BuiltinLambda>(name, wrap_comparison(op));
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

  add_comparison("<", [](int a, int b, const ConstNodePtr &node_a,
                         const ConstNodePtr &node_b) { return a < b; });
  add_comparison("<=", [](int a, int b, const ConstNodePtr &node_a,
                          const ConstNodePtr &node_b) { return a <= b; });
  add_comparison(">", [](int a, int b, const ConstNodePtr &node_a,
                         const ConstNodePtr &node_b) { return a > b; });
  add_comparison(">=", [](int a, int b, const ConstNodePtr &node_a,
                          const ConstNodePtr &node_b) { return a >= b; });

  add("=", [](const ConstNodePtr &parent,
              const std::vector<ConstNodePtr> &args) {
    if (args.size() != 2) {
      throw RuntimeError(parent->get_location(), "invalid number of arguments");
    }

    auto result = args.at(0)->equals(args.at(1));
    return make_nonnull_shared<const Bool>(parent->get_location(), result);
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

  add("empty?", [](const ConstNodePtr &parent,
                   const std::vector<ConstNodePtr> &args) {
    if (args.size() != 1) {
      throw RuntimeError(parent->get_location(), "wrong number of arguments");
    }

    auto list = args.at(0).cast<const List>();
    if (list) {
      return make_nonnull_shared<Bool>(parent->get_location(),
                                       (*list)->get_nodes().empty());
    }

    throw RuntimeError(args.at(0)->get_location(), "not a list");
  });

  add("count", [](const ConstNodePtr &parent,
                  const std::vector<ConstNodePtr> &args) {
    if (args.size() != 1) {
      throw RuntimeError(parent->get_location(), "wrong number of arguments");
    }

    auto list = args.at(0).cast<const List>();
    if (list) {
      return make_nonnull_shared<Number>(parent->get_location(),
                                         (*list)->get_nodes().size());
    }

    return make_nonnull_shared<Number>(parent->get_location(), 0);
  });

  add("prn", [](const auto &parent, const auto &args) {
    if (args.size() != 1) {
      throw RuntimeError(parent->get_location(), "wrong number of arguments");
    }

    std::cout << args.at(0)->print() << std::endl;

    return make_nonnull_shared<Nil>(parent->get_location());
  });
}

void Environment::dump() {
  for (const auto &[key, value] : this->map) {
    std::cout << "ENV: " << key << " = " << value->print() << std::endl;
  }

  if (this->parent) {
    std::cout << "---" << std::endl;
    (*this->parent)->dump();
  }
}
