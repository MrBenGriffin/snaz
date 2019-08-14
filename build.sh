#!/bin/bash
set -ev
export LOCAL_DIR="${HOME}/deps/local"
cd ${TRAVIS_BUILD_DIR}
mkdir cmake-build-debug
cd cmake-build-debug
cmake  ..
make
