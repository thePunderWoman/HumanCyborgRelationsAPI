#!/bin/sh
# Build and run the host-side tests. Needs only a C++ compiler.
set -e
here=$(cd "$(dirname "$0")" && pwd)
out=$(mktemp -d)
trap 'rm -rf "$out"' EXIT
${CXX:-c++} -std=c++14 -DARDUINO=100 -Wall \
    -I"$here/shim" -I"$here/../../src" \
    "$here/test_playwav.cpp" "$here/../../src/hcr.cpp" -o "$out/test_playwav"
"$out/test_playwav"
