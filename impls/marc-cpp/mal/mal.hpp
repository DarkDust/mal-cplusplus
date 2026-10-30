#pragma once

#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace mal {

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

  [[nodiscard]] virtual std::string print() const = 0;
};

// Type aliases for more compact/convenient handling.
using NodePtr = std::shared_ptr<Node>;
using ConstNodePtr = std::shared_ptr<const Node>;

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
  explicit Symbol(std::string str);
  [[nodiscard]] std::string print() const override;

  [[nodiscard]] const std::string &get_symbol() const;

private:
  std::string sym;
};

/**
 * AST node: a keyword.
 */
class Keyword : public Hashable {
public:
  explicit Keyword(const std::string &str);
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
  explicit String(const std::string &str);
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
  explicit Number(const std::string &str);
  explicit Number(const int num);
  [[nodiscard]] std::string print() const override;
  [[nodiscard]] int get_number() const;

private:
  int num;
};

/**
 * AST node: a list or vector.
 */
class List : public Node {
public:
  enum class Kind : std::uint8_t { List, Vector };

  explicit List(Kind kind);
  List(Kind kind, const std::vector<ConstNodePtr> &nodes);
  [[nodiscard]] std::string print() const override;

  void append(const ConstNodePtr &node);

  [[nodiscard]] const Kind get_kind() const;
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
  HashMap();
  [[nodiscard]] std::string print() const override;

  void set(const std::shared_ptr<const Hashable> &key,
           const ConstNodePtr &value);
  [[nodiscard]] const auto begin() const { return this->map.cbegin(); }
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
  Future(std::string prefix, std::string suffix, ConstNodePtr contained);
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
  [[nodiscard]] virtual std::optional<ConstNodePtr>
  eval(std::vector<ConstNodePtr> args) const = 0;
};

/**
 * Main line reader/parser implementation.
 */
class Reader final {
public:
  explicit Reader(const std::string &line);
  ~Reader();

  // Disallow copy and move.
  Reader(const Reader &) = delete;
  Reader &operator=(const Reader &) = delete;
  Reader(Reader &&) = delete;
  Reader &operator=(Reader &&) = delete;

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
  Environment();

  /** Lookup a symbol. */
  std::optional<ConstNodePtr> get(const std::string &key);

  /** Add a symbol to the environment. */
  void set(const std::string &key, ConstNodePtr val);

  /** Remove a symbol from the environment. */
  void remove(const std::string &key);

  /** Add default built-in symbols. */
  void populate_with_default();

private:
  std::unordered_map<std::string, ConstNodePtr> map;
};

} // namespace mal
