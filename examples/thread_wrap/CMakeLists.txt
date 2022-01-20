
# Set project name
set(TargetName thread_wrapper)

add_executable(${TargetName})

target_sources(${TargetName} PRIVATE
        "demoConveyorThread.h"
        "demoThread.h"
        "main.cpp"
        )
target_include_directories(${TargetName} PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(${TargetName} PRIVATE com4cpp )

install(TARGETS ${TargetName} DESTINATION bin)
