#!/usr/bin/env bash
set -euo pipefail

cd -- "$(dirname -- "${BASH_SOURCE[0]}")"

CLANG_TIDY="$(brew --prefix llvm)/bin/clang-tidy"
CLANGXX="${CLANG_TIDY%/*}/clang++"

BUILD_DIR="$(mktemp -d "${TMPDIR:-/tmp}/mal-tidy.XXXXXX")"
trap 'rm -rf -- "$BUILD_DIR"' EXIT

# Match clang-tidy's defaults instead of Homebrew's compiler configuration.
COMPILER_FLAGS=(--no-default-config -std=c++23)

# Keep the module built by Apple's Clang separate from the one used for analysis.
"$CLANGXX" "${COMPILER_FLAGS[@]}" --precompile mal/mal.cppm \
  -o "$BUILD_DIR/mal.pcm"

# Let clang-tidy discover .clang-tidy; forward any additional options to it.
"$CLANG_TIDY" "$@" mal/mal.cppm mal/*.cpp step*.cpp -- \
  "${COMPILER_FLAGS[@]}" "-fmodule-file=mal=$BUILD_DIR/mal.pcm"
