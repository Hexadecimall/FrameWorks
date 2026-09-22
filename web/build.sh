#!/bin/sh
set -eu

if [ "$#" -ne 1 ]; then
    echo "usage: web/build.sh <output-directory>" >&2
    exit 2
fi

output_directory=$1
script_directory=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
mkdir -p "$output_directory"

zig c++ "$script_directory/engine.cpp" \
    -target wasm32-freestanding \
    -nostdlib \
    -O3 \
    -Wl,--no-entry \
    -Wl,--strip-all \
    -Wl,--export=fw_engine_version \
    -Wl,--export=fw_document_width \
    -Wl,--export=fw_document_height \
    -Wl,--export=fw_document_kind \
    -Wl,--export=fw_new_document \
    -Wl,--export=fw_open_starter \
    -o "$output_directory/frameworks.wasm"

cp "$script_directory/index.html" "$script_directory/styles.css" \
   "$script_directory/app.js" "$output_directory/"
