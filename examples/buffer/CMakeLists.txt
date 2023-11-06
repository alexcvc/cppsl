set(TargetName buffer)

set(${TargetName}_SRC
   main.cpp
)

add_executable(${TargetName} ${${TargetName}_SRC})

target_include_directories(${TargetName} PUBLIC "../../include")
target_include_directories(${TargetName} PUBLIC "../../submodules/spdlog/include")
target_include_directories(${TargetName} PUBLIC "../../submodules/fmt/include")
target_link_libraries(${TargetName} cppsl fmt::fmt spdlog::spdlog )

install(TARGETS ${TargetName} DESTINATION bin)
