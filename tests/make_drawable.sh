#!/bin/bash

git clone https://github.com/5cript/cairo-wrap.git

mkdir drawings

cd cairo-wrap
mkdir build
cd build

if [ -z ${MSYSTEM_CHOST+x} ]; then
	cmake .. -G"MSYS Makefiles"
else
	cmake ..
fi

make -j4

cd ../..