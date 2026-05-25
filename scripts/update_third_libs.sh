#!/usr/bin/env bash

# Yes, I know CMake has https://cmake.org/cmake/help/latest/module/FetchContent.html
# and I have no excuse for keep using this script to download dependencies.

REMOVE_EXISTING=$1

SPDLOG_VERSION_TAG="v1.17.0"
SOCKPP_VERSION_TAG="master"
GTEST_VERSION_TAG="v1.17.0"

SPDLOG_DIR="spdlog"
SOCKPP_DIR="sockpp"
GTEST_DIR="googletest"
DIRS=("$SPDLOG_DIR" "$SOCKPP_DIR" "$GTEST_DIR")

THIRD_LIB_DIR="third"

function update_spdlog() {
    git clone --depth=1 --branch "$SPDLOG_VERSION_TAG" https://github.com/gabime/spdlog.git "$SPDLOG_DIR"
    cd "$SPDLOG_DIR" || exit
    rm -rf .git
}

function update_sockpp() {
    git clone --depth=1 --branch "$SOCKPP_VERSION_TAG" https://github.com/fpagliughi/sockpp.git "$SOCKPP_DIR"
    cd "$SOCKPP_DIR" || exit
    rm -rf .git
}

function update_gtest() {
    git clone --depth=1 --branch "$GTEST_VERSION_TAG" https://github.com/google/googletest.git "$GTEST_DIR"
    cd "$GTEST_DIR" || exit
    rm -rf .git
}

function rm_rf_dir() {
    if [ -e "$1" ];then
      rm -rf "$1"
    fi
}

clone_if_not_exist() {
    local dir="$1"
    local func_name="$2"
    if [ -e "$dir" ]; then
      echo "[$dir] exists, skip"
    else
        "$func_name"
        cd "$WD" || exit
    fi
}

# -- main --

cd "$THIRD_LIB_DIR" || exit
WD=$(pwd)

if [ -n "$REMOVE_EXISTING" ]; then
    for dir in "${DIRS[@]}"; do
        rm_rf_dir "$dir"
    done
fi

clone_if_not_exist "$SPDLOG_DIR" update_spdlog
clone_if_not_exist "$SOCKPP_DIR" update_sockpp
clone_if_not_exist "$GTEST_DIR" update_gtest
