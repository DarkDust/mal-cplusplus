module;

// Need to include all headers that are needed later on here.
// They will need to be included in the individual .hpp files again to satisfy
// clangd.
#include "linenoise.hpp"
#include <concepts>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

export module mal;

export {
#include "environment.hpp"
#include "evaluator.hpp"
#include "location.hpp"
#include "nodes/bool.hpp"
#include "nodes/builtin_lambda.hpp"
#include "nodes/future.hpp"
#include "nodes/hash_map.hpp"
#include "nodes/keyword.hpp"
#include "nodes/list.hpp"
#include "nodes/nil.hpp"
#include "nodes/node.hpp"
#include "nodes/number.hpp"
#include "nodes/string.hpp"
#include "nodes/symbol.hpp"
#include "nonnull_shared_ptr.hpp"
#include "parse_error.hpp"
#include "reader.hpp"
#include "runtime_error.hpp"
}
