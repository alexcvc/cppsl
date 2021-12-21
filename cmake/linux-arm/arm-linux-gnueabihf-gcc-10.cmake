set(CMAKE_SYSTEM_NAME               Linux)
set(CMAKE_SYSTEM_PROCESSOR          arm)

# Without that flag CMake is not able to pass test compilation check
set(CMAKE_TRY_COMPILE_TARGET_TYPE   STATIC_LIBRARY)

set(CMAKE_AR                        ${LINUX_ARM_TOOLCHAIN_10_PATH}arm-none-linux-gnueabihf-ar)
set(CMAKE_ASM_COMPILER              ${LINUX_ARM_TOOLCHAIN_10_PATH}arm-none-linux-gnueabihf-gcc)
set(CMAKE_C_COMPILER                ${LINUX_ARM_TOOLCHAIN_10_PATH}arm-none-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER              ${LINUX_ARM_TOOLCHAIN_10_PATH}arm-none-linux-gnueabihf-g++)
set(CMAKE_LINKER                    ${LINUX_ARM_TOOLCHAIN_10_PATH}arm-none-linux-gnueabihf-ld)
set(CMAKE_OBJCOPY                   ${LINUX_ARM_TOOLCHAIN_10_PATH}arm-none-linux-gnueabihf-objcopy CACHE INTERNAL "")
set(CMAKE_RANLIB                    ${LINUX_ARM_TOOLCHAIN_10_PATH}arm-none-linux-gnueabihf-ranlib CACHE INTERNAL "")
set(CMAKE_SIZE                      ${LINUX_ARM_TOOLCHAIN_10_PATH}arm-none-linux-gnueabihf-size CACHE INTERNAL "")
set(CMAKE_STRIP                     ${LINUX_ARM_TOOLCHAIN_10_PATH}arm-none-linux-gnueabihf-strip CACHE INTERNAL "")
set(CMAKE_GCOV                      ${LINUX_ARM_TOOLCHAIN_10_PATH}arm-none-linux-gnueabihf-gcov CACHE INTERNAL "")

set(CMAKE_C_FLAGS                   "${APP_C_FLAGS} -Wno-psabi" CACHE INTERNAL "")
set(CMAKE_CXX_FLAGS                 "${APP_CXX_FLAGS} ${CMAKE_C_FLAGS}" CACHE INTERNAL "")

set(CMAKE_C_FLAGS_DEBUG             "-O0 -g" CACHE INTERNAL "")
set(CMAKE_C_FLAGS_RELEASE           "-O3 -DNDEBUG" CACHE INTERNAL "")
set(CMAKE_CXX_FLAGS_DEBUG           "${CMAKE_C_FLAGS_DEBUG}" CACHE INTERNAL "")
set(CMAKE_CXX_FLAGS_RELEASE         "${CMAKE_C_FLAGS_RELEASE}" CACHE INTERNAL "")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CMAKE_C_BYTE_ORDER "LITTLE_ENDIAN")
set(CMAKE_CXX_BYTE_ORDER "LITTLE_ENDIAN")
