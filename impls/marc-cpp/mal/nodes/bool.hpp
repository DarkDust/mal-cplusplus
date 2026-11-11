#pragma once

#include "node.hpp"

namespace mal {
  /**
   * Represents the boolean true or false.
   */
  class Bool : public Node {
  public:
    /** Initialize a future.

    @param range Source location range of the node.
    @param value The boolean value.
    */
    Bool(const ConstLocationRangePtr &range, bool value);

    /** Whether the node does not represent `false` or `nil`. */
    [[nodiscard]] bool is_trueish() const override;

    /** Generate a string representation of the node. */
    [[nodiscard]] std::string print() const override;

  private:
    bool value;
  };
} // namespace mal