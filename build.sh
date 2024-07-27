#!/bin/bash

echo "------- CMAKE --------"
cd build
#cmake .. -DCMAKE_TOOLCHAIN_FILE=./toolchains/mingw.cmake
cmake .. -DCMAKE_TOOLCHAIN_FILE=./toolchains/clang.cmake

echo "------- MAKE --------"
make