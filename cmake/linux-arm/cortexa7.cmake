
# Copyright(c) 2021 Alexander Sacharov
# Distributed under the MIT License (http://opensource.org/licenses/MIT)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR armv7-a)

if(NOT DEFINED ENV{SDKTARGETSYSROOT})
message(FATAL_ERROR "'SDKTARGETSYSROOT is not defined '$ENV{SDKTARGETSYSROOT}")
endif()

if(NOT DEFINED ENV{OECORE_NATIVE_SYSROOT})
message(FATAL_ERROR "'OECORE_NATIVE_SYSROOT is not defined '$ENV{OECORE_NATIVE_SYSROOT}")
endif()

set(CMAKE_SYSROOT $ENV{SDKTARGETSYSROOT})
set(CMAKE_NATIVE_SYSROOT $ENV{OECORE_NATIVE_SYSROOT})
set(TOOLCHAIN_PREFIX arm-poky-linux-gnueabi-)

message(STATUS   "CMAKE_NATIVE_SYSROOT : ${CMAKE_NATIVE_SYSROOT}")
message(STATUS   "CMAKE_SYSROOT        : ${CMAKE_SYSROOT}")
message(STATUS   "TOOLCHAIN_PREFIX     : ${TOOLCHAIN_PREFIX}")
message(STATUS   "PATH                 : $ENV{PATH}")

set(CMAKE_AR                        ${CMAKE_SYSROOT}/usr/bin/arm-poky-linux-gnueabi/${TOOLCHAIN_PREFIX}ar)
set(CMAKE_ASM_COMPILER              ${CMAKE_SYSROOT}/usr/bin/arm-poky-linux-gnueabi/${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_C_COMPILER                ${CMAKE_NATIVE_SYSROOT}/usr/bin/arm-poky-linux-gnueabi/${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_CXX_COMPILER              ${CMAKE_NATIVE_SYSROOT}/usr/bin/arm-poky-linux-gnueabi/${TOOLCHAIN_PREFIX}g++)
set(CMAKE_LINKER                    ${CMAKE_SYSROOT}/usr/bin/arm-poky-linux-gnueabi/${TOOLCHAIN_PREFIX}ld)
set(CMAKE_OBJCOPY                   ${CMAKE_SYSROOT}/usr/bin/arm-poky-linux-gnueabi/${TOOLCHAIN_PREFIX}objcopy CACHE INTERNAL "")
set(CMAKE_RANLIB                    ${CMAKE_SYSROOT}/usr/bin/arm-poky-linux-gnueabi/${TOOLCHAIN_PREFIX}ranlib CACHE INTERNAL "")
set(CMAKE_SIZE                      ${CMAKE_SYSROOT}/usr/bin/arm-poky-linux-gnueabi/${TOOLCHAIN_PREFIX}size CACHE INTERNAL "")
set(CMAKE_STRIP                     ${CMAKE_SYSROOT}/usr/bin/arm-poky-linux-gnueabi/${TOOLCHAIN_PREFIX}strip CACHE INTERNAL "")
set(CMAKE_GCOV                      ${CMAKE_SYSROOT}/usr/bin/arm-poky-linux-gnueabi/${TOOLCHAIN_PREFIX}gcov CACHE INTERNAL "")

set(CMAKE_FIND_ROOT_PATH ${CMAKE_SYSROOT})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(COMPILER_FLAGS " -O -mthumb -mfpu=neon-vfpv4 -mfloat-abi=hard -mcpu=cortex-a7 -fstack-protector-strong")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMPILER_FLAGS}" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILER_FLAGS}" CACHE STRING "" FORCE)

set(CMAKE_C_BYTE_ORDER "LITTLE_ENDIAN")
set(CMAKE_CXX_BYTE_ORDER "LITTLE_ENDIAN")

include_directories(${CMAKE_SYSROOT}/usr/include)