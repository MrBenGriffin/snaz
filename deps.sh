#!/bin/bash
set -ev

retry=""
if [ "$1" == "travis" ]; then
  export retry=travis_retry
fi

mkdir -p ${DEPS_DIR}/libsass
cd ${DEPS_DIR}/libsass
if [ ! -f ${DEPS_DIR}/libsass/config.log ]; then
  echo "libsass incorrect"
  cd ${DEPS_DIR}
  export SASS_LIBSASS_PATH="${DEPS_DIR}/libsass"
  version="3.6.1"
  git clone -q https://github.com/sass/libsass --branch $version --depth 1 && cd libsass
  autoreconf --force --install
  ./configure --enable-shared --prefix=${LOCAL_DIR}
  make
  sudo make install
fi

bison_v=`bison -V | grep bison`
if [ "${bison_v##* }" != "3.4" ]; then
  echo "Bison ${bison_v##* } incorrect"
  cd ${DEPS_DIR}
  rm -rf ${DEPS_DIR}/bison
  mkdir -p ${DEPS_DIR}/bison
  BISON_URL="http://ftp.gnu.org/gnu/bison/bison-3.4.tar.gz"
  $retry wget --no-check-certificate --quiet -O - ${BISON_URL} | tar --strip-components=1 -xz -C bison
  cd ${DEPS_DIR}/bison
  ./configure --prefix=${LOCAL_DIR} --quiet
  sudo make install
fi

flex_v=`flex -V`
if [ "${flex_v##* }" != "2.6.3" ]; then
  echo "Flex ${flex_v##* } incorrect"
  cd ${DEPS_DIR}
  rm -rf ${DEPS_DIR}/flex
  mkdir -p ${DEPS_DIR}/flex
  FLEX_URL="https://github.com/westes/flex/releases/download/v2.6.3/flex-2.6.3.tar.gz"
  $retry wget --no-check-certificate --quiet -O - ${FLEX_URL} | tar --strip-components=1 -xz -C flex
  cd ${DEPS_DIR}/flex
  ./configure --prefix=${LOCAL_DIR} --quiet && sudo make install
fi

cmake_v=`cmake --version | grep version`
if [ "${cmake_v##* }" != "3.13.0" ]; then
  echo "Cmake ${cmake_v##* } incorrect"
  cd ${DEPS_DIR}
  rm -rf ${DEPS_DIR}/cmake
  mkdir -p ${DEPS_DIR}/cmake
  CMAKE_URL="https://cmake.org/files/v3.13/cmake-3.13.0-rc3-Linux-x86_64.tar.gz"
  $retry wget --no-check-certificate --quiet -O - ${CMAKE_URL} | tar --strip-components=1 -xz -C cmake
fi
