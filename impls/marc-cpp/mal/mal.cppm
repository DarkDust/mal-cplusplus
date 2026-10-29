module;

#include "linenoise.hpp"
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

export module mal;

// Export the declaration header so clangd can index its documentation.
export {
#include "mal.hpp"
}

namespace mal {

std::optional<std::string> read_line() {
  std::string line;
  if (linenoise::Readline("user> ", line)) {
    return std::nullopt;
  }
  return line;
}

} // namespace mal
