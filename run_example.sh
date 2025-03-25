#!/bin/bash

bash ./build_all.sh

./target/rac0 ./target/examples/int.invdev.bin -ddeb -dumpm
./target/rac0 ./target/examples/int.invinst.bin -ddeb -dumpm
./target/rac0 ./target/examples/int.invint.bin -ddeb -dumpm
./target/rac0 ./target/examples/int.invpacc.bin -ddeb -dumpm
./target/rac0 ./target/examples/int.nested.bin -ddeb -dumpm
./target/rac0 ./target/examples/int.priv.bin -ddeb -dumpm
./target/rac0 ./target/examples/int.timer.bin -ddeb -dumpm

# ./target/rac0 ./target/examples/window_fill.bin -ddeb -dsdl -dumpm
# ./target/rac0 ./target/examples/window_change.bin -ddeb -dsdl -dumpm
./target/rac0 ./target/examples/timer.bin -ddeb -dumpm