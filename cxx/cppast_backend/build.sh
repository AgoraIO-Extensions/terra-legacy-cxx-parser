#!/bin/bash

set -e
set -x

SCRIPT_PATH=$(dirname "$0")
MY_PATH=$(realpath ${SCRIPT_PATH})

BUILD_DIR=$1
TERRA_PATH=$2
ARGS=$3
OUTPUT_PATH=${BUILD_DIR}

pushd ${OUTPUT_PATH}

# set LLVM_DOWNLOAD_URL env like 
# linux: https://github.com/llvm/llvm-project/releases/download/llvmorg-15.0.6/clang+llvm-15.0.6-x86_64-linux-gnu-ubuntu-18.04.tar.xz
# macos: https://github.com/llvm/llvm-project/releases/download/llvmorg-15.0.7/clang+llvm-15.0.7-x86_64-apple-darwin21.0.tar.xz
cmake \
    -DLLVM_CONFIG_BINARY=$(which llvm-config) \
    -DLLVM_DOWNLOAD_URL=${LLVM_DOWNLOAD_URL} \
    -DRUNTIME_OUTPUT_DIRECTORY=${OUTPUT_PATH} \
    -DTERRA_PATH=${TERRA_PATH} \
    ${MY_PATH}

cmake --build .

popd

pushd ${MY_PATH}
${OUTPUT_PATH}/cppast_backend ${ARGS}
popd
