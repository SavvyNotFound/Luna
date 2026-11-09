#!/usr/bin/env bash

set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_ROOT"

BUILD_TYPE=${1:-Release}

mkdir -p build
cd build

cmake .. -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
