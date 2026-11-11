#pragma once

#include "node.hpp"

namespace mal {
  /**
   * Represents a nil value.
   */
  class Nil : public Node {
  public:
    /** Initialize a nil.

    @param range Source location range of the node.
    */
    explicit Nil(const ConstLocationRangePtr &range);

    /** Whether the node does not represent `false` or `nil`. */
    [[nodiscard]] bool is_trueish() const override;

    /** Generate a string representation of the node. */
    [[nodiscard]] std::string print() const override;
  };
} // namespace mal