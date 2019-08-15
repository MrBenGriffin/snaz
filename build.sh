#!/bin/bash
set -ev
cd ${BUILD_DIR}
mkdir cmake-build-debug
cd cmake-build-debug
cmake  ..
make
