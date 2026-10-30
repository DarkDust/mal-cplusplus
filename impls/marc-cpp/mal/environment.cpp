module;

#include <functional>
#include <memory>
#include <optional>
#include <vector>

module mal;

using namespace mal;

class BuiltinLambdaArithmetic : public BuiltinLambda {
public:
  using Operation = std::function<int(int, int)>;

  BuiltinLambdaArithmetic(std::string name, Operation op)
      : name(std::move(name)), op(std::move(op)) {};

  [[nodiscard]] std::optional<ConstNodePtr>
  eval(std::vector<ConstNodePtr> args) const override {
    if (args.size() != 2) {
      throw RuntimeError("invalid number of arguments");
    }

    auto a = dynamic_pointer_cast<const Number>(args.at(0));
    auto b = dynamic_pointer_cast<const Number>(args.at(1));
    if (!a || !b) {
      throw RuntimeError("argument is not a number");
    }

    int result = this->op(a->get_number(), b->get_number());
    auto node = std::make_shared<Number>(result);
    return node;
  }

  [[nodiscard]] std::string print() const override { return this->name; }

private:
  std::string name;
  Operation op;
};

Environment::Environment() : map{} {}

std::optional<ConstNodePtr> Environment::get(const std::string &key) {
  auto r = this->map.find(key);
  if (r != this->map.end()) {
    return (*r).second;
  } else {
    return std::nullopt;
  }
}

void Environment::set(const std::string &key, ConstNodePtr val) {
  this->map.emplace(key, val);
}

void Environment::remove(const std::string &key) { this->map.erase(key); }

void Environment::populate_with_default() {
  auto add = [this](const std::string &name,
                    BuiltinLambdaArithmetic::Operation op) {
    this->set(name,
              std::make_shared<BuiltinLambdaArithmetic>(name, std::move(op)));
  };

  add("+", std::plus<int>{});
  add("-", std::minus<int>{});
  add("*", std::multiplies<int>{});
  add("/", [](int a, int b) {
    if (b == 0) {
      throw RuntimeError("division by zero");
    }

    return a / b;
  });
}
