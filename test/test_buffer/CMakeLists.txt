set(TargetName test_buffer)

find_package(Threads REQUIRED)

# add executable
add_executable(${TargetName} main.cpp)
target_include_directories(${TargetName} PRIVATE ../../include )
target_link_libraries(${TargetName} cppsl fmt gtest gtest_main Threads::Threads)

add_test(NAME ${TargetName} COMMAND ${TargetName})
