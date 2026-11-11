module;

#include <format>
#include <iostream>
#include <optional>
#include <ranges>
#include <string>

module mal;

using namespace mal;

// String constant: "DEBUG-EVAL"
constexpr static const std::string gDebugEval = "DEBUG-EVAL";

static ConstNodePtr special_def_bang(const Evaluator &evaluator,
                                     const std::vector<ConstNodePtr> &nodes,
                                     const EnvironmentPtr &env) {
  if (nodes.size() < 2) {
    throw RuntimeError(nodes.at(0)->get_location(),
                       "wrong number of arguments");
  }
  auto key = nodes.at(1).cast<const Symbol>();
  if (!key) {
    throw RuntimeError(nodes.at(1)->get_location(),
                       "first argument to `def!` is not a symbol");
  }

  auto value = evaluator.eval(nodes.at(2), env);
  env->set((*key)->get_symbol(), value);
  return value;
}

static ConstNodePtr special_let_star(const Evaluator &evaluator,
                                     const std::vector<ConstNodePtr> &nodes,
                                     const EnvironmentPtr &env) {
  if (nodes.size() < 2) {
    throw RuntimeError(nodes.at(0)->get_location(),
                       "wrong number of arguments");
  }
  auto sub_env = make_nonnull_shared<Environment>(env);
  auto list = nodes.at(1).cast<const List>();
  if (!list) {
    throw RuntimeError(nodes.at(1)->get_location(),
                       "first argument to `let*` is not a list");
  }
  auto let_nodes = (*list)->get_nodes();
  if (let_nodes.size() % 2 != 0) {
    throw RuntimeError((*list)->get_location(),
                       "uneven list in argument to `let*`");
  }

  auto it = let_nodes.begin();
  auto end = let_nodes.end();
  while (it != end) {
    auto maybe_key = *it++;
    auto value = *it++;

    auto key = maybe_key.cast<const Symbol>();
    if (!key) {
      throw RuntimeError(maybe_key->get_location(),
                         "key is not a symbol in `let*`");
    }
    auto evaluated_value = evaluator.eval(value, sub_env);
    sub_env->set((*key)->get_symbol(), evaluated_value);
  }

  return evaluator.eval(nodes.at(2), sub_env);
}

static ConstNodePtr special_if(const Evaluator &evaluator,
                               const std::vector<ConstNodePtr> &nodes,
                               const EnvironmentPtr &env) {
  if (nodes.size() < 3) {
    throw RuntimeError(nodes.at(0)->get_location(),
                       "wrong number of arguments");
  }
  const auto &condition = nodes.at(1);
  if (evaluator.eval(condition, env)->is_trueish()) {
    return evaluator.eval(nodes.at(2), env);
  }

  if (nodes.size() < 4) {
    return make_nonnull_shared<Nil>(nodes.at(0)->get_location());
  }

  return evaluator.eval(nodes.at(3), env);
}

static ConstNodePtr special_fn_star(const Evaluator &evaluator,
                                    const std::vector<ConstNodePtr> &nodes,
                                    const EnvironmentPtr &env) {
  if (nodes.size() < 3) {
    throw RuntimeError(nodes.at(0)->get_location(),
                       "wrong number of arguments");
  }

  const auto &binds = nodes.at(1).cast<const List>();
  if (!binds) {
    throw RuntimeError(nodes.at(1)->get_location(), "not a list");
  }

  const auto &expr = nodes.at(2);

  return make_nonnull_shared<Lambda>(nodes.at(0)->get_location(),
                                     (*binds)->get_nodes(), expr, env);
}

static ConstNodePtr special_do(const Evaluator &evaluator,
                               const std::vector<ConstNodePtr> &nodes,
                               const EnvironmentPtr &env) {

  ConstNodePtr res = make_nonnull_shared<Nil>(LocationRange::create_virtual());

  for (const auto &node : nodes | std::ranges::views::drop(1)) {
    res = evaluator.eval(node, env);
  }

  return res;
}

Evaluator::Evaluator() : special_map{} {
  special_map["def!"] = special_def_bang;
  special_map["let*"] = special_let_star;
  special_map["if"] = special_if;
  special_map["fn*"] = special_fn_star;
  special_map["do"] = special_do;
}

std::optional<ConstNodePtr>
Evaluator::eval_specials(const std::string &symbol_value,
                         const std::vector<ConstNodePtr> &nodes,
                         const EnvironmentPtr &env) const {

  auto func = this->special_map.find(symbol_value);
  if (func != this->special_map.end()) {
    return func->second(*this, nodes, env);
  }

  return std::nullopt;
}

ConstNodePtr Evaluator::eval_list(const NonNullSharedPtr<const List> &list,
                                  const EnvironmentPtr &env) const {
  auto &nodes = list->get_nodes();
  if (nodes.empty()) {
    return list;
  }

  std::vector<ConstNodePtr> evaluated;
  evaluated.reserve(nodes.size());

  if (nodes.size() >= 2) {
    auto symbol = nodes.at(0).cast<const Symbol>();
    if (symbol) {
      auto symbol_value = (*symbol)->get_symbol();
      auto evaluated = this->eval_specials(symbol_value, nodes, env);
      if (evaluated) {
        return *evaluated;
      }
    }
  }

  for (auto &node : nodes) {
    evaluated.push_back(this->eval(node, env));
  }

  auto first_evaluated = evaluated.at(0);
  if (auto builtin_lambda = first_evaluated.cast<const BuiltinLambda>()) {
    evaluated.erase(evaluated.begin());
    return (*builtin_lambda)->eval(list, evaluated);

  } else if (auto lambda = first_evaluated.cast<const Lambda>()) {
    evaluated.erase(evaluated.begin());
    return (*lambda)->eval(evaluated, *this);

  } else {
    throw RuntimeError(first_evaluated->get_location(),
                       std::format("'{}' not found", first_evaluated->print()));
  }
}

ConstNodePtr Evaluator::eval_vector(const NonNullSharedPtr<const List> &list,
                                    const EnvironmentPtr &env) const {
  auto &nodes = list->get_nodes();
  std::vector<ConstNodePtr> evaluated;
  evaluated.reserve(nodes.size());

  for (auto &node : nodes) {
    evaluated.push_back(this->eval(node, env));
  }

  return make_nonnull_shared<List>(list->get_location(), List::Kind::Vector,
                                   evaluated);
}

ConstNodePtr
Evaluator::eval_hash_map(const NonNullSharedPtr<const HashMap> &map,
                         const EnvironmentPtr &env) const {
  auto result = make_nonnull_shared<HashMap>(map->get_location());

  for (auto &[key, value] : *map) {
    result->set(key, this->eval(value, env));
  }

  return result;
}

ConstNodePtr Evaluator::eval(const ConstNodePtr &node,
                             const EnvironmentPtr &env) const {
  auto debug = env->get(gDebugEval);
  if (debug && (*debug)->is_trueish()) {
    env->dump();
    std::cout << "EVAL: " << node->print() << std::endl;
  }

  auto symbol = node.cast<const Symbol>();
  if (symbol) {
    auto result = env->get((*symbol)->get_symbol());
    return result ? *result : *symbol;
  }

  auto list = node.cast<const List>();
  if (list) {
    switch ((*list)->get_kind()) {
    case List::Kind::List:
      return eval_list(*list, env);
    case List::Kind::Vector:
      return eval_vector(*list, env);
    }
  }

  auto map = node.cast<const HashMap>();
  if (map) {
    return eval_hash_map(*map, env);
  }

  return node;
}