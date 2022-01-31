
# Set project name
set(TargetName mthreads)

add_executable(${TargetName})

target_sources(${TargetName} PRIVATE "main.cpp" )
target_include_directories(${TargetName} PRIVATE ${CMAKE_SOURCE_DIR}/lib/include)
target_link_libraries(${TargetName} PRIVATE com4cpp )

install(TARGETS ${TargetName} DESTINATION bin)
