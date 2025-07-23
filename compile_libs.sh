#!/usr/bin/bash

cd ./libs/glfw
mkdir -p build
cmake -S . -B build -D GLFW_BUILD_X11=1 -D GLFW_BUILD_WAYLAND=1
cd build
make
cd ..
mkdir -p lib
cp ./build/src/libglfw3.a ./lib/libglfw3.a
cp -r ./include ./lib/include



echo finished compiling