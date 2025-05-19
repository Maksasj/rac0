#!/bin/bash

mkdir -p target

cd compiler
make
cp rac0a ../target
cd ..

cd vm
make
cp rac0 ../target
cd ..

# compile test
mkdir -p target/test
for file in assembly/test/*.asm; do
    [ -e "$file" ] || continue
    filename=$(basename "$file" .asm)
    ./target/rac0a "$file" "./target/test/${filename}.bin"
done

# compile examples
mkdir -p target/examples
for file in assembly/examples/*.asm; do
    [ -e "$file" ] || continue
    filename=$(basename "$file" .asm)
    ./target/rac0a "$file" "./target/examples/${filename}.bin"
done

# compile oc
mkdir -p target/oc
for file in assembly/oc/*.asm; do
    [ -e "$file" ] || continue
    filename=$(basename "$file" .asm)
    ./target/rac0a "$file" "./target/oc/${filename}.bin"
done
