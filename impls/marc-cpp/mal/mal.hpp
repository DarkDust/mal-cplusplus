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
  ParseError(const std::string message, int line = -1, int line_pos = -1);

private:
  int line;
  int line_pos;
};

/**
 * Virtual parent class for AST nodes.
 */
class Node {
public:
  virtual ~Node() = 0;
  virtual std::string print() const = 0;
};

/**
 * Virtual parent class for AST nodes that can be used a keys for hash maps.
 */
class Hashable : public Node {
public:
  virtual std::size_t hash() const = 0;
  virtual bool equals(const Hashable &other) const = 0;
};

/**
 * AST node: a symbol.
 */
class Symbol : public Node {
public:
  Symbol(const std::string &str);
  std::string print() const override;

private:
  std::string sym;
};

/**
 * AST node: a keyword.
 */
class Keyword : public Hashable {
public:
  Keyword(const std::string &str);
  std::string print() const override;

  std::size_t hash() const override {
    return std::hash<std::string>{}(keyword);
  }

  bool equals(const Hashable &other) const override {
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
  String(const std::string &str);
  std::string print() const override;

  std::size_t hash() const override { return std::hash<std::string>{}(str); }

  bool equals(const Hashable &other) const override {
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
  Number(const std::string &str);
  std::string print() const override;

private:
  int num;
};

/**
 * AST node: a list or vector.
 */
class List : public Node {
public:
  enum class Kind { List, Vector };

  List(Kind kind);
  std::string print() const override;

  void append(std::shared_ptr<Node> node);

private:
  Kind kind;
  std::vector<std::shared_ptr<Node>> nodes;
};

/**
 * AST node: a hash map.
 */
class HashMap : public Node {
public:
  HashMap();
  std::string print() const override;

  void set(std::shared_ptr<const Hashable> key, std::shared_ptr<Node> value);

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

  std::unordered_map<std::shared_ptr<const Hashable>, std::shared_ptr<Node>,
                     KeyHash, KeyEqual>
      map;
};

/**
 * Main line reader/parser implementation.
 */
class Reader {
public:
  Reader(const std::string &line);
  ~Reader();

  std::optional<std::shared_ptr<Node>> read_form();

private:
  // Hide the actual implementation detail.s
  struct Impl;
  std::unique_ptr<Impl> impl;
};

} // namespace mal
