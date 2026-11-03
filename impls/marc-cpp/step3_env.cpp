#include <iostream>
#include <memory>
#include <optional>
#include <string>
import mal;

std::optional<mal::ConstNodePtr> read(const std::string &line) {
  auto reader = mal::Reader(line);
  auto result = reader.read_form();
  return result;
}

std::optional<mal::ConstNodePtr>
eval(const std::optional<mal::ConstNodePtr> &node, mal::EnvironmentPtr &env);

std::optional<mal::ConstNodePtr>
eval_list(std::shared_ptr<const mal::List> &list, mal::EnvironmentPtr &env) {
  auto &nodes = list->get_nodes();
  std::vector<mal::ConstNodePtr> evaluated;
  evaluated.reserve(nodes.size());

  if (nodes.size() >= 3) {
    auto symbol = std::dynamic_pointer_cast<const mal::Symbol>(nodes.at(0));
    if (symbol && symbol->get_symbol() == "def!") {
      auto key = std::dynamic_pointer_cast<const mal::Symbol>(nodes.at(1));
      if (!key) {
        throw mal::RuntimeError("first argument to `def!` is not a symbol");
      }

      auto value = eval(nodes.at(2), env);
      if (!value) {
        throw mal::RuntimeError("empty result from eval");
      }

      env->set(key->get_symbol(), *value);
      return value;
    }

    if (symbol && symbol->get_symbol() == "let*") {
      auto sub_env = std::make_shared<mal::Environment>(env);
      auto list = std::dynamic_pointer_cast<const mal::List>(nodes.at(1));
      if (!list) {
        throw mal::RuntimeError("first argument to `let*` is not a list");
      }
      auto let_nodes = list->get_nodes();
      if (let_nodes.size() % 2 != 0) {
        throw mal::RuntimeError("uneven list in argument to `let*`");
      }

      auto it = let_nodes.begin();
      auto end = let_nodes.end();
      while (it != end) {
        auto maybe_key = *it++;
        auto value = *it++;

        auto key = std::dynamic_pointer_cast<const mal::Symbol>(maybe_key);
        if (!key) {
          throw mal::RuntimeError("key is not a symbol in `let*`");
        }
        auto evaluated_value = eval(value, sub_env);
        if (!evaluated_value) {
          throw mal::RuntimeError("empty return in eval");
        }

        sub_env->set(key->get_symbol(), *evaluated_value);
      }

      return eval(nodes.at(2), sub_env);
    }
  }

  for (auto &node : nodes) {
    auto recursed = eval(node, env);
    if (!recursed) {
      throw mal::RuntimeError("empty result from eval");
    }

    evaluated.push_back(*recursed);
  }

  auto func =
      std::dynamic_pointer_cast<const mal::BuiltinLambda>(evaluated.at(0));
  if (func && list->get_kind() == mal::List::Kind::List) {
    evaluated.erase(evaluated.begin());
    return func->eval(evaluated);
  } else {
    throw mal::RuntimeError(
        std::format("{} not found", evaluated.at(0)->print()));
  }
}

std::optional<mal::ConstNodePtr>
eval_vector(std::shared_ptr<const mal::List> &list, mal::EnvironmentPtr &env) {
  auto &nodes = list->get_nodes();
  std::vector<mal::ConstNodePtr> evaluated;
  evaluated.reserve(nodes.size());

  for (auto &node : nodes) {
    auto recursed = eval(node, env);
    if (!recursed) {
      throw mal::RuntimeError("empty result from eval");
    }

    evaluated.push_back(*recursed);
  }

  return std::make_shared<mal::List>(mal::List::Kind::Vector, evaluated);
}

std::optional<mal::ConstNodePtr>
eval_hash_map(std::shared_ptr<const mal::HashMap> &map,
              mal::EnvironmentPtr &env) {

  auto result = std::make_shared<mal::HashMap>();

  for (auto &[key, value] : *map) {
    auto evaluated = eval(value, env);
    if (!evaluated) {
      throw mal::RuntimeError("failed to evaluate hash map value");
    }

    result->set(key, *evaluated);
  }

  return result;
}

std::optional<mal::ConstNodePtr>
eval(const std::optional<mal::ConstNodePtr> &node, mal::EnvironmentPtr &env) {

  if (!node) {
    return std::nullopt;
  }

  auto symbol = std::dynamic_pointer_cast<const mal::Symbol>(*node);
  if (symbol) {
    auto result = env->get(symbol->get_symbol());
    return result ?: symbol;
  }

  auto list = std::dynamic_pointer_cast<const mal::List>(*node);
  if (list && !list->get_nodes().empty()) {
    switch (list->get_kind()) {
    case mal::List::Kind::List:
      return eval_list(list, env);
    case mal::List::Kind::Vector:
      return eval_vector(list, env);
    }
  }

  auto map = std::dynamic_pointer_cast<const mal::HashMap>(*node);
  if (map) {
    return eval_hash_map(map, env);
  }

  return node;
}

std::string print(const std::optional<mal::ConstNodePtr> &node) {
  if (!node) {
    return "";
  }
  return (*node)->print();
}

bool rep(mal::EnvironmentPtr &env) {
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
  } catch (mal::RuntimeError error) {
    std::cerr << error.what() << std::endl;
  }
  return true;
}

int main(int argc, char **argv) {
  try {
    auto env = std::make_shared<mal::Environment>();
    env->populate_with_default();

    while (rep(env)) {
    }
    return EXIT_SUCCESS;
  } catch (const std::exception &error) {
    std::cerr << "Fatal error: " << error.what() << std::endl;
  } catch (...) {
    std::cerr << "Fatal error: unknown exception" << std::endl;
  }

  return EXIT_FAILURE;
}