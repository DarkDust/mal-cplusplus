#pragma once

#include "nodes/node.hpp"

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
   * Main line reader/parser implementation.
   */
  class Reader final {
  public:
    /** Initialize with a single line. */
    Reader(const NonNullSharedPtr<Location::File> &file, int line_number,
           const std::string &line);
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

} // namespace mal