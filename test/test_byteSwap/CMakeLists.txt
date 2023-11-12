set(TargetName test_byteSwap)

# add executable
add_executable(${TargetName} main.cpp)
target_include_directories(${TargetName} PRIVATE ../../include )
target_link_libraries(${TargetName} cppsl fmt gtest gtest_main)

add_test(NAME ${TargetName} COMMAND ${TargetName})
