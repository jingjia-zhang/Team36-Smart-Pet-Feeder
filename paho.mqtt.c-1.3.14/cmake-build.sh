#!/bin/bash
# =============================================================================
# cmake-build.sh
# Bash script to automate the CMake-based build process.
# Performs configuration, compilation, testing, and packaging.
# Intended for use in local development or CI environments.
# =============================================================================

#!/bin/bash

set -e

# Clean previous build directory if it exists
rm -rf build.paho
# Create a new build directory
mkdir build.paho
# Navigate into the build directory
cd build.paho
echo "travis build dir $TRAVIS_BUILD_DIR pwd $PWD with OpenSSL root $OPENSSL_ROOT_DIR"
# Run CMake to configure the project with given options
cmake -DPAHO_BUILD_STATIC=$PAHO_BUILD_STATIC -DPAHO_BUILD_SHARED=$PAHO_BUILD_SHARED -DCMAKE_BUILD_TYPE=Debug -DPAHO_WITH_SSL=TRUE -DOPENSSL_ROOT_DIR=$OPENSSL_ROOT_DIR -DPAHO_BUILD_DOCUMENTATION=FALSE -DPAHO_BUILD_SAMPLES=TRUE -DPAHO_HIGH_PERFORMANCE=$PAHO_HIGH_PERFORMANCE -DPAHO_USE_SELECT=$PAHO_USE_SELECT ..
# Build the project
cmake --build .
# Run Python tests or post-build scripts
python3 ../test/mqttsas.py &
# Run CTest to execute unit tests
ctest -VV --timeout 600
# Run Python tests or post-build scripts
killall python3 || true
sleep 3 # allow broker time to terminate and report
#killall mosquitto
# Package the project using CPack
cpack --verbose

