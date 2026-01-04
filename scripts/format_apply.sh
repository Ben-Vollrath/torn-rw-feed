#!/usr/bin/env bash
set -euo pipefail

FILES=$(git ls-files '*.cpp' '*.cc' '*.cxx' '*.h' '*.hpp' '*.hh' '*.inl')
clang-format -i $FILES
