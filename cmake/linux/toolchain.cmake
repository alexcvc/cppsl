if (NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEFAULT_TOOLCHAIN clang)

    if (NOT DEFINED TOOLCHAIN)
        message(STATUS "'TOOLCHAIN' is not defined. Using '${DEFAULT_TOOLCHAIN}'")
        set(TOOLCHAIN ${DEFAULT_TOOLCHAIN})
    endif ()

    message(STATUS "'TOOLCHAIN' is '${TOOLCHAIN}'")

    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/${TOOLCHAIN}.cmake)

    message(STATUS "'CMAKE_TOOLCHAIN_FILE' set as '${CMAKE_TOOLCHAIN_FILE}'")

else ()
    message(STATUS "'CMAKE_TOOLCHAIN_FILE' is set already to '${CMAKE_TOOLCHAIN_FILE}'")
endif ()
