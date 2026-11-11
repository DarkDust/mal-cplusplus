#pragma once

#include "node.hpp"
#include <functional>
#include <vector>

namespace mal {
  /**
   * Built-in function node.
   */
  class BuiltinLambda : public Node {
  public:
    using Func = std::function<ConstNodePtr(
        const ConstNodePtr &parent, const std::vector<ConstNodePtr> &args)>;

    BuiltinLambda(std::string name, Func func)
        : Node(LocationRange::create_virtual()), name(std::move(name)),
          func(std::move(func)) {};

    /** Evaluate the lambda using the given arguments.
    @param parent The lambda call expression.
    @param args Argument list for the lambda.
    @returns A non-null node.
    @throws RuntimeError if the argument list has the wrong length or invalid
    content.
     */
    [[nodiscard]] ConstNodePtr
    eval(const ConstNodePtr &parent,
         const std::vector<ConstNodePtr> &args) const;

    [[nodiscard]] std::string print() const override;

  private:
    std::string name;
    Func func;
  };
} // namespace mal