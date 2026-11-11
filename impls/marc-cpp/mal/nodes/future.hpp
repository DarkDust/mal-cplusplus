#pragma once

#include "node.hpp"

namespace mal {
  /**
   * Temporary AST node to deal with unit tests until the actual nodes are
   * implemented.
   */
  class Future : public Node {
  public:
    /** Initialize a future.

    @param range Source location range of the node.
    @param prefix String to print before the "contained" node representation.
    @param suffix String to print after the "contained" node representation.
    @param contained Node to wrap.
    */
    Future(const ConstLocationRangePtr &range, std::string prefix,
           std::string suffix, const ConstNodePtr &contained);

    /** Whether the receiver and argument node are considered equal. */
    [[nodiscard]] bool
    equals(const NonNullSharedPtr<const Node> &other) const override;

    /** Generate a string representation of the node. */
    [[nodiscard]] std::string print() const override;

  private:
    std::string prefix;
    std::string suffix;
    ConstNodePtr contained;
  };
} // namespace mal