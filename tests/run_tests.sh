#!/bin/bash

cd /workspace/image-resizer-app/build/

cmake -DRUN_TESTS=ON -DCMAKE_BUILD_TYPE=Debug .. && make
ctest --output-on-failure --progress && lcov --capture --directory . --output-file coverage.info
lcov --remove coverage.info '/usr/*' --output-file coverage.info

mkdir -p /mnt/coverage && cp coverage.info /mnt/coverage/coverage.info


