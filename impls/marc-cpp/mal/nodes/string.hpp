#pragma once

#include "node.hpp"

namespace mal {
  /**
   * AST node: a string.
   */
  class String : public Hashable {
  public:
    /** Initialize with a string value. */
    String(const ConstLocationRangePtr &location, const std::string &str,
           bool unescape = true);

    /** Whether the receiver and argument node are considered equal. */
    [[nodiscard]] bool
    equals(const NonNullSharedPtr<const Node> &other) const override;

    /** Generate a string representation of the node. */
    [[nodiscard]] std::string print() const override;

    /** Generate a string from the node. */
    [[nodiscard]] std::string print_verbatim() const override;

    [[nodiscard]] std::size_t hash() const override {
      return std::hash<std::string>{}(str);
    }

    [[nodiscard]] bool equals(const Hashable &other) const override {
      auto rhs = dynamic_cast<const String *>(&other);
      return rhs && str == rhs->str;
    }

  private:
    std::string str;
  };
} // namespace mal