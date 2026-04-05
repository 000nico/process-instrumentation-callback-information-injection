#!/bin/bash

set -e

SRC="payload.cpp"
BIN_DIR="bin"

mkdir -p "$BIN_DIR"

x86_64-w64-mingw32-g++ \
    -O1 \
    -nostdlib \
    -fno-stack-protector \
    -fno-exceptions \
    -fno-rtti \
    -masm=intel \
    -c "$SRC" -o "$BIN_DIR/payload.o"

x86_64-w64-mingw32-g++ \
    -nostdlib \
    -Wl,--entry=callback \
    "$BIN_DIR/payload.o" \
    -o "$BIN_DIR/combined.exe"

objcopy -O binary \
    --only-section=.text \
    "$BIN_DIR/combined.exe" "$BIN_DIR/payload.bin"

xxd -i "$BIN_DIR/payload.bin" \
    | sed 's/unsigned char/static unsigned char/g; s/unsigned int/static unsigned int/g' \
    > "$BIN_DIR/shellcode.hpp"

echo "Done - $(wc -c < "$BIN_DIR/payload.bin") bytes"

rm -f "$BIN_DIR/payload.o" "$BIN_DIR/combined.exe" "$BIN_DIR/payload.bin"