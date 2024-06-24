#!/bin/bash

PROJECTNAME="TRIPLE_PILOT"

cd build
cmake .. 
make

cd ..

rm -rf ./vm/iso/*
rsync -r --info=progress2 ./build/bin/* ./vm/iso
#rsync -r --info=progress2 ./vm/isoextra/* ./vm/iso

genisoimage -input-charset utf-8 ./vm/iso ./vm/isoextra > ./vm/iso.iso