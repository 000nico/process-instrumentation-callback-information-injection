#!/bin/bash
mkdir -p bin
x86_64-w64-mingw32-g++ \
    -O1 \
    -nostdlib \
    -fno-stack-protector \
    -fno-exceptions \
    -fno-rtti \
    -fPIC \
    -c wrapper.cpp -o bin/wrapper.o

objdump -d bin/wrapper.o

objcopy -O binary \
    --only-section=.text \
    bin/wrapper.o bin/wrapper.bin
xxd -i bin/wrapper.bin | sed 's/unsigned char/static unsigned char/g; s/unsigned int/static unsigned int/g' > bin/wrapper.hpp
echo "Done - $(wc -c < bin/wrapper.bin) bytes"
rm -f bin/wrapper.o bin/wrapper.bin