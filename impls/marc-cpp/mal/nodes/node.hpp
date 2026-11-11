#pragma once

#include "../location.hpp"
#include "../nonnull_shared_ptr.hpp"
#include <string>

namespace mal {

  /**
   * Parent class for AST nodes.
   */
  class Node {
  public:
    explicit Node(const ConstLocationRangePtr &location);
    virtual ~Node() = 0;

    // Disallow copy and move.
    Node(const Node &) = delete;
    Node &operator=(const Node &) = delete;
    Node(Node &&) = delete;
    Node &operator=(Node &&) = delete;

    /** Generate a string representation of the node. */
    [[nodiscard]] virtual std::string print() const = 0;

    /** Get the node's location range in the source. */
    [[nodiscard]] const ConstLocationRangePtr &get_location() const;

    /** Set the node's location range in the source. */
    void set_location(const ConstLocationRangePtr &location);

    /** Whether the node does not represent `false` or `nil`. */
    [[nodiscard]] virtual bool is_trueish() const;

    /** Whether the receiver and argument node are considered equal. */
    [[nodiscard]] virtual bool
    equals(const NonNullSharedPtr<const Node> &other) const = 0;

  private:
    ConstLocationRangePtr location;
  };

  /**
   * Virtual parent class for AST nodes that can be used a keys for hash maps.
   */
  class Hashable : public Node {
  public:
    explicit Hashable(const ConstLocationRangePtr &location) : Node(location) {}

    [[nodiscard]] virtual std::size_t hash() const = 0;
    [[nodiscard]] virtual bool equals(const Hashable &other) const = 0;
  };

  // Type aliases for more compact/convenient handling.
  using NodePtr = NonNullSharedPtr<Node>;
  using ConstNodePtr = NonNullSharedPtr<const Node>;
} // namespace mal