### Set project name
set(TargetName cppsl)

### List of CPP (source) library files.
file(GLOB_RECURSE ${TargetName}_SRC *.cpp)

### List of HPP (header) library files.
file(GLOB_RECURSE ${TargetName}_HDR ${CMAKE_CURRENT_SOURCE_DIR}/../include/*.h*)

### add executable
add_library(${TargetName} ${${TargetName}_SRC} ${${TargetName}_HDR})

# add include directories
target_include_directories(${TargetName}
   PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
   PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>"
   PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../submodules/spdlog/include>"
   PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../submodules/fmt/include>"
   "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>")

set_target_properties(${TargetName} PROPERTIES
   PUBLIC_HEADER "${${TargetName}_HDR}"
)

#target_link_libraries(${TargetName} PRIVATE fmt::fmt-header-only)
#target_link_libraries(${TargetName} PRIVATE fmt::fmt)
#target_link_libraries(${TargetName} PRIVATE fmt::fmt spdlog::spdlog)

# definitions
if (${IS_BIG_ENDIAN})
   target_compile_definitions(${TargetName} PUBLIC "-D_BENDIAN")
else ()
   target_compile_definitions(${TargetName} PUBLIC "-D_LENDIAN")
endif ()

target_compile_options(${TargetName} PRIVATE
   "-Wall"
   "-Wextra"
   "-Werror=return-type")
