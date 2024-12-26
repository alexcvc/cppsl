set(TargetName test_container)

# add executable
add_executable(${TargetName} main.cpp)
target_include_directories(${TargetName} PRIVATE ../../include)
target_link_libraries(${TargetName} cppsl fmt)

add_test(NAME ${TargetName} COMMAND ${TargetName})
