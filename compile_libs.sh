#!/usr/bin/bash

SCRIPT_DIR="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

cd $SCRIPT_DIR/libs/glfw
mkdir -p build
cmake -S . -B build -D GLFW_BUILD_WAYLAND=ON
cd build
make
cd ..
mkdir -p lib
cp ./build/src/libglfw3.a ./lib/libglfw3.a
cp -r ./include ./lib/include

cd $SCRIPT_DIR/luajit
make PREFIX=$SCRIPT_DIR/libs/Lua
make install PREFIX=$SCRIPT_DIR/libs/Lua


echo "finished compiling into ${SCRIPT_DIR}/libs"
