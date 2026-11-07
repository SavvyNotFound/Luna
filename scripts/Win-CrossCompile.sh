#!/usr/bin/env bash

set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_ROOT"

BUILD_TYPE=${1:-Debug}

mkdir -p build-windows
cd build-windows

cmake .. \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DCMAKE_SYSTEM_NAME=Windows \
    -DCMAKE_SYSTEM_PROCESSOR=AMD64 \
    -DCMAKE_C_COMPILER="$HOME/opt/msvc/bin/x64/cl" \
    -DCMAKE_CXX_COMPILER="$HOME/opt/msvc/bin/x64/cl"
