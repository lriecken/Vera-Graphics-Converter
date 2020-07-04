#!/bin/bash

rm -rf build
mkdir build
cd build
qmake ../src/VeraGraphicsConverter.pro
make -j4
cd ..
