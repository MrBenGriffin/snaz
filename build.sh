#!/bin/bash
#set -ev
cd ${BUILD_DIR}
mkdir -p cmake-build-debug
mkdir -p cmake-build-release
cd cmake-build-release
cmake  ..
make
