#pragma once

#include "location.hpp"
#include <stdexcept>
#include <string>

namespace mal {
  /**
   * Runtime error when evaluation fails.
   */
  class RuntimeError : public std::runtime_error {
  public:
    RuntimeError(const ConstLocationRangePtr &location,
                 const std::string &message);

    [[nodiscard]] std::string print() const;

  private:
    ConstLocationRangePtr location;
  };
} // namespace mal