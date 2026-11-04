#pragma once

#include "location.hpp"
#include <stdexcept>
#include <string>

namespace mal {

  /**
   * Runtime error when parsing fails.
   */
  class ParseError : public std::runtime_error {
  public:
    ParseError(const ConstLocationRangePtr &location,
               const std::string &message);

    [[nodiscard]] std::string print() const;

  private:
    ConstLocationRangePtr location;
  };

} // namespace mal