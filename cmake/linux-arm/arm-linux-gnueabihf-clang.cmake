set(CMAKE_SYSTEM_NAME               Linux)
set(CMAKE_SYSTEM_PROCESSOR          arm)

# Without this flag CMake is not able to pass the compiler sanity check.
set(CMAKE_TRY_COMPILE_TARGET_TYPE   STATIC_LIBRARY)

set(CMAKE_AR                        llvm-ar)
set(CMAKE_ASM_COMPILER              clang)
set(CMAKE_C_COMPILER                clang)
set(CMAKE_CXX_COMPILER              clang++)
set(CMAKE_LINKER                    lld)
set(CMAKE_OBJCOPY                   llvm-objcopy CACHE INTERNAL "")
set(CMAKE_RANLIB                    llvm-ranlib CACHE INTERNAL "")
set(CMAKE_SIZE                      llvm-size CACHE INTERNAL "")
set(CMAKE_STRIP                     llvm-strip CACHE INTERNAL "")
set(CMAKE_GCOV                      llvm-cov CACHE INTERNAL "")

get_filename_component(TOOLCHAIN_ROOT "${LINUX_ARM_TOOLCHAIN_10_PATH}" DIRECTORY)
set(CMAKE_C_FLAGS                   "${APP_C_FLAGS} -target arm-none-linux-gnueabihf --gcc-toolchain=${TOOLCHAIN_ROOT} --sysroot=${TOOLCHAIN_ROOT}/arm-none-linux-gnueabihf/libc" CACHE INTERNAL "")
set(CMAKE_CXX_FLAGS                 "${APP_CXX_FLAGS} ${CMAKE_C_FLAGS}" CACHE INTERNAL "")

set(CMAKE_C_FLAGS_DEBUG             "-O0 -g" CACHE INTERNAL "")
set(CMAKE_C_FLAGS_RELEASE           "-O3 -DNDEBUG" CACHE INTERNAL "")
set(CMAKE_CXX_FLAGS_DEBUG           "${CMAKE_C_FLAGS_DEBUG}" CACHE INTERNAL "")
set(CMAKE_CXX_FLAGS_RELEASE         "${CMAKE_C_FLAGS_RELEASE}" CACHE INTERNAL "")

set(CMAKE_EXE_LINKER_FLAGS          "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=${LINUX_ARM_TOOLCHAIN_10_PATH}arm-none-linux-gnueabihf-ld")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CMAKE_C_BYTE_ORDER "LITTLE_ENDIAN")
set(CMAKE_CXX_BYTE_ORDER "LITTLE_ENDIAN")
