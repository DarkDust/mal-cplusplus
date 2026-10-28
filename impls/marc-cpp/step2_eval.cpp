#include <iostream>
#include <memory>
#include <optional>
#include <string>
import mal;

std::optional<std::shared_ptr<mal::Node>> read(const std::string &line) {
  auto reader = new mal::Reader(line);
  auto result = reader->read_form();
  delete reader;
  return result;
}

std::optional<std::shared_ptr<mal::Node>>
eval(const std::optional<std::shared_ptr<mal::Node>> &node,
     std::shared_ptr<mal::Environment> &env) {

  if (!node) {
    return std::nullopt;
  }

  auto symbol = std::dynamic_pointer_cast<mal::Symbol>(*node);
  if (symbol) {
    auto result = env->get(symbol->get_symbol());
    return result ?: symbol;
  }

  auto list = std::dynamic_pointer_cast<mal::List>(*node);
  if (list && !list->get_nodes().empty()) {
    auto &nodes = list->get_nodes();
    std::vector<std::shared_ptr<mal::Node>> evaluated;
    evaluated.reserve(nodes.size());

    for (auto &node : nodes) {
      auto recursed = eval(node, env);
      if (!recursed) {
        throw mal::RuntimeError("empty result from eval");
      }

      evaluated.push_back(*recursed);
    }

    auto func = std::dynamic_pointer_cast<mal::BuiltinLambda>(evaluated[0]);
    if (func) {
      evaluated.erase(evaluated.begin());
      return func->eval(evaluated);
    } else {
      return std::make_shared<mal::List>(mal::List::Kind::List, evaluated);
    }
  }

  return node;
}

std::string print(const std::optional<std::shared_ptr<mal::Node>> &node) {
  if (!node) {
    return "";
  }
  return (*node)->print();
}

bool rep(std::shared_ptr<mal::Environment> &env) {
  auto line = mal::read_line();
  if (!line) {
    return false;
  }

  try {
    auto intermediate_read = read(*line);
    auto intermediate_eval = eval(intermediate_read, env);
    auto intermediate_print = print(intermediate_eval);
    std::cout << intermediate_print << std::endl;
  } catch (mal::ParseError error) {
    std::cerr << error.what() << std::endl;
  }
  return true;
}

int main(int argc, char **argv) {
  auto env = std::make_shared<mal::Environment>();
  env->populate_with_default();

  while (rep(env))
    ;
  return 0;
}