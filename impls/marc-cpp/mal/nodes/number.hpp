#pragma once

#include "node.hpp"

namespace mal {
  /**
   * AST node: a number.
   */
  class Number : public Node {
  public:
    /** Initialize with a string representation. */
    Number(const ConstLocationRangePtr &location, const std::string &str);
    /** Initialize with a number. */
    Number(const ConstLocationRangePtr &location, const int num);

    /** Generate a string representation of the node. */
    [[nodiscard]] std::string print() const override;

    /** Get the represented number. */
    [[nodiscard]] int get_number() const;

  private:
    int num;
  };
} // namespace mal