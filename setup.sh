#!/bin/bash

CMAKE_ARGS=(-G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release)
BUILD_ARGS=(--parallel 8)
DEPENCENCY_DIR=dependencies

check_dir() {
    [ ! -d "$1" ] && mkdir $1 && return 0 || return 1
}

build() {
    check_dir $1
    [ $? -gt 0 ] && echo "$1 already exists" && return
    git clone $2 src/$1
    cmake "${CMAKE_ARGS[@]}" -S src/$1 -B src/$1 -DCMAKE_INSTALL_PREFIX=./$1
    cmake --build src/$1 --target install "${BUILD_ARGS[@]}"
}

check_dir $DEPENCENCY_DIR
cd $DEPENCENCY_DIR

check_dir src
build logger https://github.com/lukaswagner/logger.git
build glfw https://github.com/glfw/glfw

rm -rf src
