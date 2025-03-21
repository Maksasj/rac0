#!/bin/bash

mkdir -p target

cd compiler
make
cp rac0a ../target
cd ..

# compile failing
for file in assembly/test/failing/*.asm; do
    [ -e "$file" ] || continue
    filename=$(basename "$file" .asm)
    ./target/rac0a "$file" "o.bin"
done