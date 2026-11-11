#pragma once

#include "node.hpp"
#include <vector>

namespace mal {

  /**
   * AST node: a list or vector.
   */
  class List : public Node {
  public:
    /** Whether the node represents a list or vector. */
    enum class Kind : std::uint8_t { List, Vector };

    /** Initialize with a kind and empty nodes list. */
    List(const ConstLocationRangePtr &location, Kind kind);

    /** Initialize with a kind and a given nodes list.
    @param range Source location range of the node.
    @param kind Whether the node represents a list or vector.
    @param nodes List of nodes.
     */
    List(const ConstLocationRangePtr &location, Kind kind,
         const std::vector<ConstNodePtr> &nodes);

    /** Whether the receiver and argument node are considered equal. */
    [[nodiscard]] bool
    equals(const NonNullSharedPtr<const Node> &other) const override;

    /** Generate a string representation of the node. */
    [[nodiscard]] std::string print() const override;

    /** Append a node to the list or vector.

    @param node Node to append.
    */
    void append(const ConstNodePtr &node);

    /** Get the kind. */
    [[nodiscard]] const Kind get_kind() const;

    /** Get the list of nodes.

    Guaranteed to not contain null pointers.
    */
    [[nodiscard]] const std::vector<ConstNodePtr> &get_nodes() const;

  private:
    Kind kind;
    std::vector<ConstNodePtr> nodes;
  };
} // namespace mal