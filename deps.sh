#!/bin/bash
set -ev

function vGood()
{
 base=$1
 args=( "$@" )
 small=$(IFS=$'\n'; echo "${args[*]}" | sort -V | head -n1 )
 if [ "${base}" == "${small}" ]; then
   echo 1
 else
   echo 0
 fi
}

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

vstring=$(bison -V | grep bison)
if [ 0 -eq $(vGood 3.4.1 ${vstring##* }) ]; then
  echo "Bison ${vstring##* }  too low"
  cd ${DEPS_DIR}
  rm -rf ${DEPS_DIR}/bison
  mkdir -p ${DEPS_DIR}/bison
  BISON_URL="http://ftp.gnu.org/gnu/bison/bison-3.4.1.tar.gz"
  $retry wget --no-check-certificate --quiet -O - ${BISON_URL} | tar --strip-components=1 -xz -C bison
  cd ${DEPS_DIR}/bison
  ./configure --prefix=${LOCAL_DIR} --quiet
  sudo make install
fi

vstring=$(flex -V)
if [ 0 -eq $(vGood 2.6.3 ${vstring##* }) ]; then
  echo "Flex ${vstring##* }  too low"
  cd ${DEPS_DIR}
  rm -rf ${DEPS_DIR}/flex
  mkdir -p ${DEPS_DIR}/flex
  FLEX_URL="https://github.com/westes/flex/releases/download/v2.6.3/flex-2.6.3.tar.gz"
  $retry wget --no-check-certificate --quiet -O - ${FLEX_URL} | tar --strip-components=1 -xz -C flex
  cd ${DEPS_DIR}/flex
  ./configure --prefix=${LOCAL_DIR} --quiet && sudo make install
fi

vstring=$(cmake --version | grep version)
if [ 0 -eq $(vGood 3.13.0 ${vstring##* }) ]; then
  echo "Cmake ${vstring##* } too low"
  cd ${DEPS_DIR}
  rm -rf ${DEPS_DIR}/cmake
  mkdir -p ${DEPS_DIR}/cmake
  CMAKE_URL="https://cmake.org/files/v3.13/cmake-3.13.0-rc3-Linux-x86_64.tar.gz"
  $retry wget --no-check-certificate --quiet -O - ${CMAKE_URL} | tar --strip-components=1 -xz -C cmake
fi

cd ${TRAVIS_BUILD_DIR}
