#!/usr/bin/env bash
set -euo pipefail

# Adjust paths to your project layout
FILES=$(git ls-files '*.cpp' '*.cc' '*.cxx' '*.h' '*.hpp' '*.hh' '*.inl')

# Check mode (no changes). Fails if formatting differs.
clang-format -n -Werror $FILES
