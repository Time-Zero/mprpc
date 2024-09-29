# !/bin/bash

set -e

mkdir `pwd`/build
cd `pwd`/build
cmake ..
make -j$(nproc)

cd ..
cp -r `pwd`/src/include `pwd`/lib
