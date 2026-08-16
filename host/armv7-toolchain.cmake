# Cross toolchain for armv7 (Luckfox Lyra, RK3506, hard-float ABI).
#
# The cross libc/libstdc++ come from Ubuntu's g++-arm-linux-gnueabihf
# (sysroot under /usr/arm-linux-gnueabihf); SDL2 comes from the
# device's own libsdl2-dev, pulled into host/sysroot.

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER   arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)

set(CMAKE_FIND_ROOT_PATH /usr/arm-linux-gnueabihf)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# RP3506: dual Cortex-A7 + Cortex-M0. Tune for A7; VFPv4 with NEON is
# available but the math code paths are scalar double, so keep it off
# to avoid ABI surprises with the prebuilt SDL2.
add_compile_options(-march=armv7ve+fp -mfpu=neon-vfpv4 -mfloat-abi=hard -mtune=cortex-a7)
