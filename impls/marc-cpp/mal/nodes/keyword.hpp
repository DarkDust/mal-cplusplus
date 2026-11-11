#pragma once

#include "node.hpp"

namespace mal {
  /**
   * AST node: a keyword.
   */
  class Keyword : public Hashable {
  public:
    /** Initialize with an explicit keyword name. */
    explicit Keyword(const ConstLocationRangePtr &location,
                     const std::string &str);

    /** Whether the receiver and argument node are considered equal. */
    [[nodiscard]] bool
    equals(const NonNullSharedPtr<const Node> &other) const override;

    /** Generate a string representation of the node. */
    [[nodiscard]] std::string print() const override;

    [[nodiscard]] std::size_t hash() const override {
      return std::hash<std::string>{}(keyword);
    }

    [[nodiscard]] bool equals(const Hashable &other) const override {
      auto rhs = dynamic_cast<const Keyword *>(&other);
      return rhs && keyword == rhs->keyword;
    }

  private:
    std::string keyword;
  };
} // namespace mal