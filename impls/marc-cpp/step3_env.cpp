#include <iostream>
#include <optional>
#include <string>
import mal;

// String constant: "DEBUG-EVAL"
static const std::string gDebugEval = "DEBUG-EVAL";

std::optional<mal::ConstNodePtr>
read(const mal::NonNullSharedPtr<mal::Location::File> &file, int line_number,
     const std::string &line) {
  auto reader = mal::Reader(file, line_number, line);
  auto result = reader.read_form();
  return result;
}

std::optional<mal::ConstNodePtr>
eval(const std::optional<mal::ConstNodePtr> &node, mal::EnvironmentPtr &env);

std::optional<mal::ConstNodePtr>
eval_specials(const std::string &symbol_value,
              const std::vector<mal::ConstNodePtr> &nodes,
              mal::EnvironmentPtr &env) {
  if (symbol_value == "def!") {
    auto key = nodes.at(1).cast<const mal::Symbol>();
    if (!key) {
      throw mal::RuntimeError(nodes.at(1)->get_location(),
                              "first argument to `def!` is not a symbol");
    }

    auto value = eval(nodes.at(2), env);
    if (!value) {
      throw mal::RuntimeError(nodes.at(2)->get_location(),
                              "empty result from eval");
    }

    env->set((*key)->get_symbol(), *value);
    return value;
  }

  if (symbol_value == "let*") {
    auto sub_env = mal::make_nonnull_shared<mal::Environment>(env);
    auto list = nodes.at(1).cast<const mal::List>();
    if (!list) {
      throw mal::RuntimeError(nodes.at(1)->get_location(),
                              "first argument to `let*` is not a list");
    }
    auto let_nodes = (*list)->get_nodes();
    if (let_nodes.size() % 2 != 0) {
      throw mal::RuntimeError((*list)->get_location(),
                              "uneven list in argument to `let*`");
    }

    auto it = let_nodes.begin();
    auto end = let_nodes.end();
    while (it != end) {
      auto maybe_key = *it++;
      auto value = *it++;

      auto key = maybe_key.cast<const mal::Symbol>();
      if (!key) {
        throw mal::RuntimeError(maybe_key->get_location(),
                                "key is not a symbol in `let*`");
      }
      auto evaluated_value = eval(value, sub_env);
      if (!evaluated_value) {
        throw mal::RuntimeError(value->get_location(), "empty return in eval");
      }

      sub_env->set((*key)->get_symbol(), *evaluated_value);
    }

    return eval(nodes.at(2), sub_env);
  }

  return std::nullopt;
}

std::optional<mal::ConstNodePtr>
eval_list(mal::NonNullSharedPtr<const mal::List> &list,
          mal::EnvironmentPtr &env) {
  auto &nodes = list->get_nodes();
  if (nodes.empty()) {
    return list;
  }

  std::vector<mal::ConstNodePtr> evaluated;
  evaluated.reserve(nodes.size());

  if (nodes.size() >= 3) {
    auto symbol = nodes.at(0).cast<const mal::Symbol>();
    if (symbol) {
      auto symbol_value = (*symbol)->get_symbol();
      auto evaluated = eval_specials(symbol_value, nodes, env);
      if (evaluated) {
        return evaluated;
      }
    }
  }

  for (auto &node : nodes) {
    auto recursed = eval(node, env);
    if (!recursed) {
      throw mal::RuntimeError(node->get_location(), "empty result from eval");
    }

    evaluated.push_back(*recursed);
  }

  auto first_evaluated = evaluated.at(0);
  auto func = first_evaluated.cast<const mal::BuiltinLambda>();
  if (func && list->get_kind() == mal::List::Kind::List) {
    evaluated.erase(evaluated.begin());
    return (*func)->eval(evaluated);
  } else {
    throw mal::RuntimeError(
        first_evaluated->get_location(),
        std::format("'{}' not found", first_evaluated->print()));
  }
}

std::optional<mal::ConstNodePtr>
eval_vector(mal::NonNullSharedPtr<const mal::List> &list,
            mal::EnvironmentPtr &env) {
  auto &nodes = list->get_nodes();
  std::vector<mal::ConstNodePtr> evaluated;
  evaluated.reserve(nodes.size());

  for (auto &node : nodes) {
    auto recursed = eval(node, env);
    if (!recursed) {
      throw mal::RuntimeError(node->get_location(), "empty result from eval");
    }

    evaluated.push_back(*recursed);
  }

  return mal::make_nonnull_shared<mal::List>(
      list->get_location(), mal::List::Kind::Vector, evaluated);
}

std::optional<mal::ConstNodePtr>
eval_hash_map(mal::NonNullSharedPtr<const mal::HashMap> &map,
              mal::EnvironmentPtr &env) {

  auto result = mal::make_nonnull_shared<mal::HashMap>(map->get_location());

  for (auto &[key, value] : *map) {
    auto evaluated = eval(value, env);
    if (!evaluated) {
      throw mal::RuntimeError(value->get_location(),
                              "failed to evaluate hash map value");
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

  auto debug = env->get(gDebugEval);
  if (debug && (*debug)->is_trueish()) {
    std::cout << "EVAL: " << (*node)->print() << std::endl;
  }

  auto symbol = node->cast<const mal::Symbol>();
  if (symbol) {
    auto result = env->get((*symbol)->get_symbol());
    return result ?: symbol;
  }

  auto list = node->cast<const mal::List>();
  if (list) {
    switch ((*list)->get_kind()) {
    case mal::List::Kind::List:
      return eval_list(*list, env);
    case mal::List::Kind::Vector:
      return eval_vector(*list, env);
    }
  }

  auto map = node->cast<const mal::HashMap>();
  if (map) {
    return eval_hash_map(*map, env);
  }

  return node;
}

std::string print(const std::optional<mal::ConstNodePtr> &node) {
  if (!node) {
    return "";
  }
  return (*node)->print();
}

bool rep(const mal::NonNullSharedPtr<mal::Location::File> &file,
         int line_number, mal::EnvironmentPtr &env) {
  auto line = mal::read_line();
  if (!line) {
    return false;
  }

  try {
    auto intermediate_read = read(file, line_number, *line);
    auto intermediate_eval = eval(intermediate_read, env);
    auto intermediate_print = print(intermediate_eval);
    std::cout << intermediate_print << std::endl;
  } catch (mal::ParseError error) {
    std::cerr << error.print() << std::endl;
  } catch (mal::RuntimeError error) {
    std::cerr << error.print() << std::endl;
  }
  return true;
}

int main(int argc, char **argv) {
  try {
    auto file = mal::make_nonnull_shared<mal::Location::File>("<stdin>");
    int line_number = 1;

    auto env = mal::make_nonnull_shared<mal::Environment>();
    env->populate_with_default();

    while (rep(file, line_number++, env)) {
    }
    return EXIT_SUCCESS;
  } catch (const std::exception &error) {
    std::cerr << "Fatal error: " << error.what() << std::endl;
  } catch (...) {
    std::cerr << "Fatal error: unknown exception" << std::endl;
  }

  return EXIT_FAILURE;
}