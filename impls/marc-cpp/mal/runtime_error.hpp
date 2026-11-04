#pragma once

#include <stdexcept>
#include <string>

namespace mal {
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
} // namespace mal