if (NOT DEFINED PLATFORM)
    set(DEFAULT_PLATFORM  linux)
    message(STATUS "'PLATFORM' is not defined. Supported linux, linux-arm")
    message(STATUS "Using '${DEFAULT_PLATFORM}'")
    set(PLATFORM            ${DEFAULT_PLATFORM})
else()
    message(STATUS "'PLATFORM is '${PLATFORM}'")
endif ()

# Setup platform toolchain file.
include(cmake/${PLATFORM}/toolchain.cmake)
