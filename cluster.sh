#!/bin/bash
export BUILD_DIR=$(pwd)
export DEPS_DIR="${HOME}/deps"
export LOCAL_DIR="/usr/local"
mkdir -p ${DEPS_DIR}/cmake/bin
export PATH=${LOCAL_DIR}/bin:${DEPS_DIR}/cmake/bin:${PATH}
source deps.sh
cd ${BUILD_DIR}
source build.sh
