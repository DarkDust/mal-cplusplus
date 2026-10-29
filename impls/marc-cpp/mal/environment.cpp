module;

#include <memory>
#include <optional>
#include <vector>

module mal;

using namespace mal;

class BuiltinLambdaPlus : public BuiltinLambda {
public:
  std::optional<std::shared_ptr<Node>>
  eval(std::vector<std::shared_ptr<Node>> args) override {
    if (args.size() != 2) {
      throw RuntimeError("invalid number of arguments");
    }

    auto a = dynamic_pointer_cast<Number>(args[0]);
    auto b = dynamic_pointer_cast<Number>(args[1]);
    if (!a || !b) {
      throw RuntimeError("argument is not a number");
    }

    int result = a->get_number() + b->get_number();
    auto node = std::make_shared<Number>(result);
    return node;
  }

  [[nodiscard]] std::string print() const override { return "+"; }
};

class BuiltinLambdaMinus : public BuiltinLambda {
public:
  std::optional<std::shared_ptr<Node>>
  eval(std::vector<std::shared_ptr<Node>> args) override {
    if (args.size() != 2) {
      throw RuntimeError("invalid number of arguments");
    }

    auto a = dynamic_pointer_cast<Number>(args[0]);
    auto b = dynamic_pointer_cast<Number>(args[1]);
    if (!a || !b) {
      throw RuntimeError("argument is not a number");
    }

    int result = a->get_number() - b->get_number();
    auto node = std::make_shared<Number>(result);
    return node;
  }

  [[nodiscard]] std::string print() const override { return "-"; }
};

class BuiltinLambdaMultiply : public BuiltinLambda {
public:
  std::optional<std::shared_ptr<Node>>
  eval(std::vector<std::shared_ptr<Node>> args) override {
    if (args.size() != 2) {
      throw RuntimeError("invalid number of arguments");
    }

    auto a = dynamic_pointer_cast<Number>(args[0]);
    auto b = dynamic_pointer_cast<Number>(args[1]);
    if (!a || !b) {
      throw RuntimeError("argument is not a number");
    }

    int result = a->get_number() * b->get_number();
    auto node = std::make_shared<Number>(result);
    return node;
  }

  [[nodiscard]] std::string print() const override { return "*"; }
};

class BuiltinLambdaDivide : public BuiltinLambda {
public:
  std::optional<std::shared_ptr<Node>>
  eval(std::vector<std::shared_ptr<Node>> args) override {
    if (args.size() != 2) {
      throw RuntimeError("invalid number of arguments");
    }

    auto a = dynamic_pointer_cast<Number>(args[0]);
    auto b = dynamic_pointer_cast<Number>(args[1]);
    if (!a || !b) {
      throw RuntimeError("argument is not a number");
    }

    int result = a->get_number() / b->get_number();
    auto node = std::make_shared<Number>(result);
    return node;
  }

  [[nodiscard]] std::string print() const override { return "/"; }
};

Environment::Environment() : map{} {}

std::optional<std::shared_ptr<Node>> Environment::get(const std::string &key) {
  auto r = this->map.find(key);
  if (r != this->map.end()) {
    return (*r).second;
  } else {
    return std::nullopt;
  }
}

void Environment::set(const std::string &key, std::shared_ptr<Node> val) {
  this->map.emplace(key, val);
}

void Environment::remove(const std::string &key) { this->map.erase(key); }

void Environment::populate_with_default() {
  this->set("+", std::make_shared<BuiltinLambdaPlus>());
  this->set("-", std::make_shared<BuiltinLambdaMinus>());
  this->set("*", std::make_shared<BuiltinLambdaMultiply>());
  this->set("/", std::make_shared<BuiltinLambdaDivide>());
}
