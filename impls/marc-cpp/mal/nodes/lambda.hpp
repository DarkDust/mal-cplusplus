#pragma once

#include "../environment.hpp"
#include "../evaluator.hpp"
#include "node.hpp"
#include <vector>

namespace mal {
  /**
   * Represents a callable function/lambda.
   */
  class Lambda : public Node {
  public:
    /** Initialize a nil.

    @param range Source location range of the node.
    @param binds The lambda argument list.
    @param expr Expression node to evaluate.
    @param env Environment at definition site.
    */
    Lambda(const ConstLocationRangePtr &range, std::vector<ConstNodePtr> binds,
           const ConstNodePtr &expr, const EnvironmentPtr &env);

    /** Evaluate the lambda using the given arguments and environment. */
    [[nodiscard]] ConstNodePtr eval(const std::vector<ConstNodePtr> &args,
                                    const Evaluator &evaluator) const;

    /** Whether the receiver and argument node are considered equal. */
    [[nodiscard]] bool
    equals(const NonNullSharedPtr<const Node> &other) const override;

    /** Generate a string representation of the node. */
    [[nodiscard]] std::string print() const override;

  private:
    std::vector<ConstNodePtr> binds;
    EnvironmentPtr env;
    ConstNodePtr expr;
  };
} // namespace mal