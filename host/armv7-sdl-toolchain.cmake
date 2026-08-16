# Cross toolchain for building the static SDL2 dependency (armv7,
# Luckfox Lyra). Same triple as armv7-toolchain.cmake, but the CMake
# find roots also include host/sysroot so SDL's configure step can see
# the X11 headers and link stubs pulled from the device. X11 itself is
# dlopen'd at runtime (SDL_X11_SHARED), so nothing links against those
# stubs in the end.

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER   arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)

set(CMAKE_FIND_ROOT_PATH /usr/arm-linux-gnueabihf ${CMAKE_CURRENT_LIST_DIR}/sysroot)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

add_compile_options(-march=armv7ve+fp -mfpu=neon-vfpv4 -mfloat-abi=hard -mtune=cortex-a7)
