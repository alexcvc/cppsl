
# enable test
enable_testing()

# GOOGLE test
add_subdirectory(lib)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_subdirectory(test_byteSwap)
add_subdirectory(test_enumBitMask)
add_subdirectory(test_fileAppender)
add_subdirectory(test_results)
add_subdirectory(test_buffer)
add_subdirectory(test_timers)
