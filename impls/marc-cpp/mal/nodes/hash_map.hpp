#pragma once

#include "node.hpp"
#include <unordered_map>

namespace mal {
  /**
   * AST node: a hash map.
   */
  class HashMap : public Node {
  public:
    /** Initialize an empty hash map. */
    HashMap();

    /** Generate a string representation of the node. */
    [[nodiscard]] std::string print() const override;

    /** Add an entry to the hash map.

    @param key A symbol or keyword. Must not be null.
    @param value A value. Must not be null.
     */
    void set(const std::shared_ptr<const Hashable> &key,
             const ConstNodePtr &value);

    /** Begin-Iterator for the key/value pairs. */
    [[nodiscard]] const auto begin() const { return this->map.cbegin(); }
    /** End-Iterator for the key/value pairs. */
    [[nodiscard]] const auto end() const { return this->map.cend(); }

  private:
    struct KeyHash {
      std::size_t operator()(const std::shared_ptr<const Hashable> &key) const {
        return key->hash();
      }
    };

    struct KeyEqual {
      bool operator()(const std::shared_ptr<const Hashable> &a,
                      const std::shared_ptr<const Hashable> &b) const {
        return a->equals(*b);
      }
    };

    std::unordered_map<std::shared_ptr<const Hashable>, ConstNodePtr, KeyHash,
                       KeyEqual>
        map;
  };
} // namespace mal