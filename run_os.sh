#!/bin/bash

bash build_all.sh

./target/rac0a "./assembly/oc/rac0oc.asm" "./target/oc/rac0oc.bin" -dump
./target/rac0 ./target/oc/rac0oc.bin -ddeb -dumpm -dfile