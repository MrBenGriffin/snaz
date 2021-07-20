#!/bin/bash
#set -ev
if [ -z ${DEPS_DIR} ]; then
  echo "DEPS_DIR is not set"
  return 1
fi
if [ -z ${LOCAL_DIR} ]; then
  echo "LOCAL_DIR is not set"
  return 1
fi

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
  version="3.6.5"
  git clone -q https://github.com/sass/libsass --branch $version --depth 1 && cd libsass
  autoreconf --force --install
  ./configure --enable-shared --prefix=${LOCAL_DIR}
  make
  sudo make install
fi

vstring=$(bison -V | grep bison)
if [ 0 -eq $(vGood 3.4.1 ${vstring##* }) ]; then
  echo "Bison ${vstring##* }  too low"
  if [ ! -f ${DEPS_DIR}/bison/src/bison ]; then
    echo "Bison dep not found."
    rm -rf ${DEPS_DIR}/bison
    mkdir -p ${DEPS_DIR}/bison
    cd ${DEPS_DIR}
    BISON_URL="http://ftp.gnu.org/gnu/bison/bison-3.7.tar.gz"
    $retry wget --no-check-certificate --quiet -O - ${BISON_URL} | tar --strip-components=1 -xz -C bison
  fi
  cd ${DEPS_DIR}/bison
  ./configure --prefix=${LOCAL_DIR} --quiet
  sudo make install
fi

vstring=$(flex -V)
if [ 0 -eq $(vGood 2.6.3 ${vstring##* }) ]; then
  echo "Flex ${vstring##* }  too low"
  if [ ! -f ${DEPS_DIR}/flex/src/flex ]; then
    echo "Flex dep not found."
    rm -rf ${DEPS_DIR}/flex
    mkdir -p ${DEPS_DIR}/flex
    cd ${DEPS_DIR}
    FLEX_URL="https://github.com/westes/flex/releases/download/v2.6.3/flex-2.6.3.tar.gz"
    $retry wget --no-check-certificate --quiet -O - ${FLEX_URL} | tar --strip-components=1 -xz -C flex
  fi
  cd ${DEPS_DIR}/flex
  ./configure --prefix=${LOCAL_DIR} --quiet
  sudo make install
fi

vstring=$(cmake --version | grep version)
if [ 0 -eq $(vGood 3.21.0 ${vstring##* }) ]; then
  echo "Cmake ${vstring##* } too low"
  if [ ! -f ${DEPS_DIR}/cmake/bin/cmake ]; then
    echo "Cmake dep not found."
    cd ${DEPS_DIR}
    rm -rf ${DEPS_DIR}/cmake
    mkdir -p ${DEPS_DIR}/cmake
    CMAKE_URL="https://github.com/Kitware/CMake/releases/download/v3.21.0/cmake-3.21.0.tar.gz"
    $retry wget --no-check-certificate --quiet -O - ${CMAKE_URL} | tar --strip-components=1 -xz -C cmake
  fi
fi
