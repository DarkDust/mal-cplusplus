#pragma once

#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace mal {

// Forward declarations
template <class T> class NonNullSharedPtr;

template <class T, class... Args>
NonNullSharedPtr<T> make_nonnull_shared(Args &&...args);

/** Represents a shared non-null pointer. */
template <class T> class NonNullSharedPtr {
public:
  /** Conversion initializer. */
  template <class U>
    requires std::convertible_to<std::shared_ptr<U>, std::shared_ptr<T>>
  explicit(false) NonNullSharedPtr(const NonNullSharedPtr<U> &other)
      : ptr(other.shared()) {}

  ~NonNullSharedPtr() = default;

  // Allow copy.
  NonNullSharedPtr(const NonNullSharedPtr &) = default;
  NonNullSharedPtr &operator=(const NonNullSharedPtr &) = default;
  // Prevent move.
  // NonNullSharedPtr(NonNullSharedPtr &&) = delete;
  // NonNullSharedPtr &operator=(NonNullSharedPtr &&) = delete;

  T &operator*() const { return *ptr; }
  T *operator->() const { return ptr.get(); }

  // Return a copy, so callers cannot reset our stored pointer.
  [[nodiscard]] std::shared_ptr<T> shared() const { return ptr; }

  /** Try to cast the shared pointer to a target type. */
  template <class Target> std::optional<NonNullSharedPtr<Target>> cast() const {
    auto target = std::dynamic_pointer_cast<Target>(ptr);
    if (target) {
      return std::optional<NonNullSharedPtr<Target>>{
          NonNullSharedPtr<Target>(target)};
    } else {
      return std::nullopt;
    }
  };

private:
  /** Initialize with an existing shared pointer. */
  explicit NonNullSharedPtr(std::shared_ptr<T> ptr) : ptr(std::move(ptr)) {
    if (!this->ptr) {
      throw std::invalid_argument("null pointer");
    }
  }

  std::shared_ptr<T> ptr;

  template <class U, class... Args>
  friend NonNullSharedPtr<U> make_nonnull_shared(Args &&...args);

  template <class> friend class NonNullSharedPtr;
};

/** Generate a non-null shared pointer. */
template <class T, class... Args>
NonNullSharedPtr<T> make_nonnull_shared(Args &&...args) {
  auto intermediate = std::make_shared<T>(std::forward<Args>(args)...);
  return NonNullSharedPtr(intermediate);
}

/**
 * Read a line from standard input.
 *
 * Displays the "user> " prompt and supports line editing.
 *
 * @return The entered line, or std::nullopt on end-of-input.
 */
std::optional<std::string> read_line();

/**
 * Runtime error when parsing fails.
 */
class ParseError : public std::runtime_error {
public:
  explicit ParseError(const std::string &message, int line = -1,
                      int line_pos = -1);

private:
  int line;
  int line_pos;
};

/**
 * Runtime error when evaluation fails.
 */
class RuntimeError : public std::runtime_error {
public:
  explicit RuntimeError(const std::string &message, int line = -1,
                        int line_pos = -1);

private:
  int line;
  int line_pos;
};

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

// Type aliases for more compact/convenient handling.
using NodePtr = NonNullSharedPtr<Node>;
using ConstNodePtr = NonNullSharedPtr<const Node>;

/**
 * Virtual parent class for AST nodes that can be used a keys for hash maps.
 */
class Hashable : public Node {
public:
  [[nodiscard]] virtual std::size_t hash() const = 0;
  [[nodiscard]] virtual bool equals(const Hashable &other) const = 0;
};

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

/**
 * AST node: a keyword.
 */
class Keyword : public Hashable {
public:
  /** Initialize with an explicit keyword name. */
  explicit Keyword(const std::string &str);

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

/**
 * AST node: a string.
 */
class String : public Hashable {
public:
  /** Initialize with a string value. */
  explicit String(const std::string &str);

  /** Generate a string representation of the node. */
  [[nodiscard]] std::string print() const override;

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

/**
 * AST node: a list or vector.
 */
class List : public Node {
public:
  /** Whether the node represents a list or vector. */
  enum class Kind : std::uint8_t { List, Vector };

  /** Initialize with a kind and empty nodes list. */
  explicit List(Kind kind);
  /** Initialize with a kind and a given nodes list.

  @param kind Whether the node represents a list or vector.
  @param nodes List of nodes.
   */
  List(Kind kind, const std::vector<ConstNodePtr> &nodes);

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

/**
 * Temporary AST node to deal with unit tests until the actual nodes are
 * implemented.
 */
class Future : public Node {
public:
  /** Initialize a future.

  @param prefix String to print before the "contained" node representation.
  @param suffix String to print after the "contained" node representation.
  @param contained Node to wrap.
  */
  Future(std::string prefix, std::string suffix, const ConstNodePtr &contained);

  /** Generate a string representation of the node. */
  [[nodiscard]] std::string print() const override;

private:
  std::string prefix;
  std::string suffix;
  ConstNodePtr contained;
};

/**
 * Base class for built-in functions.
 */
class BuiltinLambda : public Node {
public:
  /** Evaluate the lambda using the given arguments.

  @param args Argument list for the lambda.
  @returns A non-null node.
  @throws RuntimeError if the argument list has the wrong length or invalid
  content.
   */
  [[nodiscard]] virtual ConstNodePtr
  eval(std::vector<ConstNodePtr> args) const = 0;
};

/**
 * Main line reader/parser implementation.
 */
class Reader final {
public:
  /** Initialize with a single line. */
  explicit Reader(const std::string &line);
  ~Reader();

  // Disallow copy and move.
  Reader(const Reader &) = delete;
  Reader &operator=(const Reader &) = delete;
  Reader(Reader &&) = delete;
  Reader &operator=(Reader &&) = delete;

  /** Parse a "form" from the input.

  @return A new node or null (for example on empty input).
  @throws ParseError on invalid input.
  */
  std::optional<ConstNodePtr> read_form();

private:
  // Hide the actual implementation detail.s
  struct Impl;
  std::unique_ptr<Impl> impl;
};

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

  /** Lookup a node in the receiver. Walks up the parent hierarchy when no node
  was found.

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

private:
  std::optional<NonNullSharedPtr<Environment>> parent;
  std::unordered_map<std::string, ConstNodePtr> map;
};

using EnvironmentPtr = NonNullSharedPtr<Environment>;

} // namespace mal
