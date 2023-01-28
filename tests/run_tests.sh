#!/bin/bash

cd /workspace/image-resizer-app/build/
curl -Os https://uploader.codecov.io/latest/linux/codecov
chmod +x codecov

cmake -DRUN_TESTS=ON -DCMAKE_BUILD_TYPE=Debug .. && make
ctest --output-on-failure --progress && lcov --capture --directory . --output-file coverage.info
lcov --remove coverage.info '/usr/*' --output-file coverage.info

./codecov -f coverage.info || echo "Codecov did not collect coverage reports"
