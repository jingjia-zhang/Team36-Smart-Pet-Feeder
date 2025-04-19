# ============================================================================
# toolchain.win32.cmake
# Toolchain file for building 32-bit Windows binaries using CMake.
# Intended for native or cross-compilation with MinGW or similar toolchains.
# Usage: cmake -DCMAKE_TOOLCHAIN_FILE=toolchain.win32.cmake ..
# ============================================================================

# Name of the target platform
SET(CMAKE_SYSTEM_NAME Windows)

# Version of the system
SET(CMAKE_SYSTEM_VERSION 1)

# specify the cross compiler
SET(CMAKE_C_COMPILER i686-w64-mingw32-gcc)
SET(CMAKE_CXX_COMPILER i686-w64-mingw32-g++)
SET(CMAKE_RC_COMPILER_ENV_VAR "RC")
SET(CMAKE_RC_COMPILER "")
SET(CMAKE_SHARED_LINKER_FLAGS
    "-fdata-sections -ffunction-sections -Wl,--enable-stdcall-fixup -static-libgcc -static -lpthread" CACHE STRING "" FORCE)
SET(CMAKE_EXE_LINKER_FLAGS
    "-fdata-sections -ffunction-sections -Wl,--enable-stdcall-fixup -static-libgcc -static -lpthread" CACHE STRING "" FORCE)
