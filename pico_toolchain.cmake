
# CMake toolchain file for Raspberry Pi Pico development
# This file sets up the cross-compilation environment for the arm-none-eabi-gcc toolchain

# Set the system name and processor
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Toolchain prefix
set(TOOLCHAIN_PREFIX arm-none-eabi)

# Compilers
set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PREFIX}-gcc)

# Optional: GDB for debugging
set(CMAKE_GDB ${TOOLCHAIN_PREFIX}-gdb)

# Specify the target architecture
set(CMAKE_C_FLAGS "-mcpu=cortex-m0plus -mthumb -mthumb-interwork")
set(CMAKE_CXX_FLAGS "-mcpu=cortex-m0plus -mthumb -mthumb-interwork")

# Specify the path to the Pico SDK
set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})

# Include the Pico SDK
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Don't look for standard system libraries
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Don't try to compile a test program
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

#message(STATUS "PICO_SDK_PATH = ${PICO_SDK_PATH}")