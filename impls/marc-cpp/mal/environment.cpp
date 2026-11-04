module;

#include <functional>
#include <optional>
#include <vector>

module mal;

using namespace mal;

class BuiltinLambdaArithmetic : public BuiltinLambda {
public:
  using Operation =
      std::function<int(int, int, const ConstNodePtr &, const ConstNodePtr &)>;

  BuiltinLambdaArithmetic(const ConstLocationRangePtr &location,
                          std::string name, Operation op)
      : BuiltinLambda(location), name(std::move(name)), op(std::move(op)) {};

  [[nodiscard]] ConstNodePtr
  eval(std::vector<ConstNodePtr> args) const override {
    if (args.size() != 2) {
      throw RuntimeError(this->get_location(), "invalid number of arguments");
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

    int result = this->op((*a)->get_number(), (*b)->get_number(), *a, *b);
    return make_nonnull_shared<Number>(LocationRange::create_virtual(), result);
  }

  [[nodiscard]] std::string print() const override { return this->name; }

private:
  std::string name;
  Operation op;
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
  auto add = [this](const std::string &name,
                    BuiltinLambdaArithmetic::Operation op) {
    auto func = make_nonnull_shared<BuiltinLambdaArithmetic>(
        LocationRange::create_virtual(), name, std::move(op));
    this->set(name, func);
  };

  add("+", [](int a, int b, const ConstNodePtr &node_a,
              const ConstNodePtr &node_b) { return a + b; });
  add("-", [](int a, int b, const ConstNodePtr &node_a,
              const ConstNodePtr &node_b) { return a - b; });
  add("*", [](int a, int b, const ConstNodePtr &node_a,
              const ConstNodePtr &node_b) { return a * b; });
  add("/",
      [](int a, int b, const ConstNodePtr &node_a, const ConstNodePtr &node_b) {
        if (b == 0) {
          throw RuntimeError(node_b->get_location(), "division by zero");
        }

        return a / b;
      });
}
