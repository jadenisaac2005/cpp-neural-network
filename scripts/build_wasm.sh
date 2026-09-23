#!/usr/bin/env bash
# Rebuilds both Wasm outputs from the same C++ sources:
#   docs/     - UMD build (script-tag friendly) for the GitHub Pages demo
#   dist/es6/ - ES module build for bundlers (e.g. a Next.js client component)
#
# Requires an activated Emscripten SDK (source emsdk_env.sh first).
set -euo pipefail
cd "$(dirname "$0")/.."

SRC="src/Matrix.cpp src/Layer.cpp src/Network.cpp src/bindings.cpp"
COMMON_FLAGS=(
    -O3 --bind
    -s MODULARIZE=1
    -s EXPORT_NAME=createModule
    -s ALLOW_MEMORY_GROWTH=1
    -s "EXPORTED_FUNCTIONS=['_malloc','_free']"
    -s "EXPORTED_RUNTIME_METHODS=['HEAPU8','FS']"
    -I include
)

echo "Building docs/ (UMD)..."
emcc "${COMMON_FLAGS[@]}" -o docs/network.js $SRC

echo "Building dist/es6/ (ES module)..."
mkdir -p dist/es6
emcc "${COMMON_FLAGS[@]}" -s EXPORT_ES6=1 -s ENVIRONMENT=web -o dist/es6/network.js $SRC

echo "Done."
