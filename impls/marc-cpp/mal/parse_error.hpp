#pragma once

#include <stdexcept>
#include <string>

namespace mal {

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

} // namespace mal