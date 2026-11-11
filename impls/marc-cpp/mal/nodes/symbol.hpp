#pragma once

#include "node.hpp"

namespace mal {
  /**
   * AST node: a symbol.
   */
  class Symbol : public Node {
  public:
    /** Initialize with an explicit symbol name. */
    Symbol(const ConstLocationRangePtr &location, std::string str);

    /** Whether the receiver and argument node are considered equal. */
    [[nodiscard]] bool
    equals(const NonNullSharedPtr<const Node> &other) const override;

    /** Generate a string representation of the node. */
    [[nodiscard]] std::string print() const override;

    /** Query the symbol name. */
    [[nodiscard]] const std::string &get_symbol() const;

  private:
    std::string sym;
  };
} // namespace mal