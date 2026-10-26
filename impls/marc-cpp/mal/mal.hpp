#pragma once

#include <optional>
#include <string>

namespace mal {

/**
 * Read a line from standard input.
 *
 * Displays the "user> " prompt and supports line editing.
 *
 * @return The entered line, or std::nullopt on end-of-input.
 */
std::optional<std::string> read_line();

} // namespace mal
