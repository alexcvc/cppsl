# add executable
add_executable(fft-complex-test fft-complex-test.cpp)
target_include_directories(fft-complex-test PRIVATE ../../include )
target_link_libraries(fft-complex-test cppsl fmt gtest gtest_main)

add_test(NAME fft-cpp-complex-test COMMAND fft-complex-test)

# add executable
add_executable(fft-real-pair-test fft-real-pair-test.c)
target_include_directories(fft-real-pair-test PRIVATE ../../include )
target_link_libraries(fft-real-pair-test cppsl fmt gtest gtest_main)

add_test(NAME fft-real-pair-test COMMAND fft-real-pair-test)
