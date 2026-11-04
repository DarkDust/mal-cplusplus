#pragma once

#include "node.hpp"

namespace mal {
  /**
   * AST node: a number.
   */
  class Number : public Node {
  public:
    /** Initialize with a string representation. */
    explicit Number(const std::string &str);
    /** Initialize with a number. */
    explicit Number(const int num);

    /** Generate a string representation of the node. */
    [[nodiscard]] std::string print() const override;

    /** Get the represented number. */
    [[nodiscard]] int get_number() const;

  private:
    int num;
  };
} // namespace mal