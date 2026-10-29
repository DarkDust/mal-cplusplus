module;

#include <stdexcept>
#include <string>

module mal;

using namespace mal;

RuntimeError::RuntimeError(const std::string &message, int line, int line_pos)
    : std::runtime_error{message}, line(line), line_pos(line_pos) {}
