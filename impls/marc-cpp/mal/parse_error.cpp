module;

#include <stdexcept>
#include <string>

module mal;

using namespace mal;

ParseError::ParseError(const std::string message, int line, int line_pos)
    : std::runtime_error{message} {
  this->line = line;
  this->line_pos = line_pos;
}
