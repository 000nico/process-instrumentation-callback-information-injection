#!/bin/bash

set -e

SRC="wrapper.cpp"
BIN_DIR="bin"

mkdir -p "$BIN_DIR"

x86_64-w64-mingw32-g++ \
    -O1 \
    -nostdlib \
    -fno-stack-protector \
    -fno-exceptions \
    -fno-rtti \
    -masm=intel \
    -c "$SRC" -o "$BIN_DIR/wrapper.o"

objdump -d "$BIN_DIR/wrapper.o"

objcopy -O binary \
    --only-section=.text \
    "$BIN_DIR/wrapper.o" "$BIN_DIR/wrapper.bin"

xxd -i "$BIN_DIR/wrapper.bin" \
    | sed 's/unsigned char/static unsigned char/g; s/unsigned int/static unsigned int/g' \
    > "$BIN_DIR/wrapper.hpp"

echo "Done - $(wc -c < "$BIN_DIR/wrapper.bin") bytes"

rm -f "$BIN_DIR/wrapper.o" "$BIN_DIR/wrapper.bin"