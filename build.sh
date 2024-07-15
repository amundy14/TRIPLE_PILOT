#!/bin/bash

PROJECTNAME="TRIPLE_PILOT"

echo "------- CMAKE --------"
cd build
#cmake .. -DCMAKE_TOOLCHAIN_FILE=./toolchains/mingw.cmake
cmake .. -DCMAKE_TOOLCHAIN_FILE=./toolchains/clang.cmake

echo "------- MAKE --------"
make

cd ..

echo "------- RSYNC --------"
rm -rf ./vm/iso/*
rsync -r --info=progress2 ./build/bin/* ./vm/iso

echo "------- GENISOIMAGE --------"
genisoimage -input-charset utf-8 ./vm/iso ./vm/isoextra > ./vm/iso.iso