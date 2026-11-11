#pragma once

#include "environment.hpp"
#include "nodes/hash_map.hpp"
#include "nodes/list.hpp"
#include "nodes/node.hpp"
#include <optional>

namespace mal {
  class Evaluator {
  public:
    Evaluator() = default;

    /** Evalate a node.
     * @param node Node to evaluate.
     * @param env Environment for current evaluation.
     * @returns Evaluation result.
     * @throws RuntimeError on evaluation exception.
     */
    [[nodiscard]] ConstNodePtr eval(const ConstNodePtr &node,
                                    const EnvironmentPtr &env) const;

  private:
    [[nodiscard]] std::optional<ConstNodePtr>
    eval_specials(const std::string &symbol_value,
                  const std::vector<ConstNodePtr> &nodes,
                  const EnvironmentPtr &env) const;

    [[nodiscard]] ConstNodePtr
    eval_list(const NonNullSharedPtr<const List> &list,
              const EnvironmentPtr &env) const;

    [[nodiscard]] ConstNodePtr
    eval_vector(const NonNullSharedPtr<const List> &list,
                const EnvironmentPtr &env) const;

    [[nodiscard]] ConstNodePtr
    eval_hash_map(const NonNullSharedPtr<const HashMap> &map,
                  const EnvironmentPtr &env) const;
  };
} // namespace mal