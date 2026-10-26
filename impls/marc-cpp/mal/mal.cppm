module;

#include <string>
#include "linenoise.hpp"

export module mal;

export namespace mal
{
    /** Read a line from `stdin`.
     *
     * @return Line or `std::nullopt` on end-of-stream.
     */
    std::optional<std::string> read_line()
    {
        std::string line;
        if (linenoise::Readline("user> ", line))
        {
            return std::nullopt;
        }
        return line;
    }
}