#pragma once

#include "node.hpp"

namespace mal {
  /**
   * AST node: a symbol.
   */
  class Symbol : public Node {
  public:
    /** Initialize with an explicit symbol name. */
    explicit Symbol(std::string str);

    /** Generate a string representation of the node. */
    [[nodiscard]] std::string print() const override;

    /** Query the symbol name. */
    [[nodiscard]] const std::string &get_symbol() const;

  private:
    std::string sym;
  };
} // namespace mal