set(TargetName test_thread_wakeup)

# add executable
add_executable(${TargetName} main.cpp )
target_link_libraries(${TargetName} fmt)
