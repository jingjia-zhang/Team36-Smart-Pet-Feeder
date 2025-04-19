# ========================================================================
# toolchain.linux-arm11.cmake
# Toolchain file for cross-compiling to ARM11 Linux target using CMake.
# Use this with: cmake -DCMAKE_TOOLCHAIN_FILE=toolchain.linux-arm11.cmake ..
# Define target system name, compiler paths, and root filesystems.
# ========================================================================

# path to compiler and utilities
# specify the cross compiler
SET(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)

# Name of the target platform
SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_PROCESSOR arm)

# Version of the system
SET(CMAKE_SYSTEM_VERSION 1)
