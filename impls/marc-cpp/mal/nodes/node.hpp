#pragma once

#include "../nonnull_shared_ptr.hpp"
#include <string>

namespace mal {

  /**
   * Virtual parent class for AST nodes.
   */
  class Node {
  public:
    Node() = default;
    virtual ~Node() = 0;

    // Disallow copy and move.
    Node(const Node &) = delete;
    Node &operator=(const Node &) = delete;
    Node(Node &&) = delete;
    Node &operator=(Node &&) = delete;

    /** Generate a string representation of the node. */
    [[nodiscard]] virtual std::string print() const = 0;
  };

  /**
   * Virtual parent class for AST nodes that can be used a keys for hash maps.
   */
  class Hashable : public Node {
  public:
    [[nodiscard]] virtual std::size_t hash() const = 0;
    [[nodiscard]] virtual bool equals(const Hashable &other) const = 0;
  };

  // Type aliases for more compact/convenient handling.
  using NodePtr = NonNullSharedPtr<Node>;
  using ConstNodePtr = NonNullSharedPtr<const Node>;
} // namespace mal