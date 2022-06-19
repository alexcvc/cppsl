set(TEST_TARGET_NAME test_doctest)


if(NOT ${CMAKE_CURRENT_BINARY_DIR}/doctest.h )
set(url https://raw.githubusercontent.com/onqtam/doctest/v2.4.8/doctest/doctest.h)
file(
  DOWNLOAD ${url} "${CMAKE_CURRENT_BINARY_DIR}/doctest.h"
  STATUS status)
list(GET status 0 error)
if(error)
  message(FATAL_ERROR "Could not download ${url}")
endif()
endif()

set(${TEST_TARGET_NAME}_SRC main.cpp)

find_package(Threads REQUIRED)

add_executable(${TEST_TARGET_NAME} ${${TEST_TARGET_NAME}_SRC})
target_link_libraries(${TEST_TARGET_NAME} Threads::Threads)
target_include_directories(${TEST_TARGET_NAME} PRIVATE
        ${doctest_INCLUDE_DIR}
        ${spdlog_INCLUDE_DIR}
        )
add_test(NAME ${TEST_TARGET_NAME}
        COMMAND ${TEST_TARGET_NAME}
        WORKING_DIRECTORY $<TARGET_FILE_DIR:${TEST_TARGET_NAME}>)