#include <iostream>
#include <optional>
#include <string>
import mal;

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
eval_list(mal::NonNullSharedPtr<const mal::List> &list,
          mal::EnvironmentPtr &env) {
  auto &nodes = list->get_nodes();
  if (nodes.empty()) {
    return list;
  }

  std::vector<mal::ConstNodePtr> evaluated;
  evaluated.reserve(nodes.size());

  for (auto &node : nodes) {
    auto recursed = eval(node, env);
    if (!recursed) {
      throw mal::RuntimeError(node->get_location(), "empty result from eval");
    }

    evaluated.push_back(*recursed);
  }

  auto func = evaluated.at(0).cast<const mal::BuiltinLambda>();
  if (func && list->get_kind() == mal::List::Kind::List) {
    evaluated.erase(evaluated.begin());
    return (*func)->eval(evaluated);
  } else {
    return mal::make_nonnull_shared<mal::List>(list->get_location(),
                                               list->get_kind(), evaluated);
  }
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

  auto symbol = node->cast<const mal::Symbol>();
  if (symbol) {
    auto result = env->get((*symbol)->get_symbol());
    return result ?: symbol;
  }

  auto list = node->cast<const mal::List>();
  if (list) {
    return eval_list(*list, env);
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