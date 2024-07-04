#!/bin/bash

PROJECTNAME="TRIPLE_PILOT"

cd build
#cmake .. -DCMAKE_TOOLCHAIN_FILE=./toolchains/mingw.cmake
cmake .. -DCMAKE_TOOLCHAIN_FILE=./toolchains/clang.cmake
make

cd ..

rm -rf ./vm/iso/*
rsync -r --info=progress2 ./build/bin/* ./vm/iso

genisoimage -input-charset utf-8 ./vm/iso ./vm/isoextra > ./vm/iso.iso