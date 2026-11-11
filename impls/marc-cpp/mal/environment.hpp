#pragma once

#include "nodes/node.hpp"
#include <unordered_map>

namespace mal {
  /**
   * An execution environment.
   */
  class Environment {
  public:
    /** Initialize with an optional parent environemt.

    @param parent The parent environment. May be null.
     */
    explicit Environment(
        std::optional<NonNullSharedPtr<Environment>> parent = std::nullopt);

    /** Lookup a node in the receiver. Walks up the parent hierarchy when no
    node was found.

    @param key Lookup key.
    @returns A node or nullopt if there is none with the given key.
    */
    [[nodiscard]] std::optional<ConstNodePtr> get(const std::string &key) const;

    /** Add a symbol to the environment.

    @param key Lookup key.
    @param value Node to store. Must not be null.
    */
    void set(const std::string &key, ConstNodePtr value);

    /** Remove a symbol from the environment. */
    void remove(const std::string &key);

    /** Add default built-in symbols. */
    void populate_with_default();

    /** Print debug output. */
    void dump();

  private:
    std::optional<NonNullSharedPtr<Environment>> parent;
    std::unordered_map<std::string, ConstNodePtr> map;
  };

  using EnvironmentPtr = NonNullSharedPtr<Environment>;

} // namespace mal