cmake_minimum_required(VERSION 3.5)

# Set the PROJECT_NAME, PROJECT_VERSION as well as other variable
project(cppsl_lib
   DESCRIPTION "C++20 support library"
   LANGUAGES CXX C
   VERSION 2.0.0)

### Options
option(BUILD_SHARED_LIBS "Build libraries as shared as opposed to static" ON)
option(BUILD_TESTING "Build tests" OFF)
option(BUILD_EXAMPLES "Build examples" OFF)
option(BUILD_DOC "Build doxygen documentation" OFF)

# Defines the CMAKE_INSTALL_LIBDIR, CMAKE_INSTALL_BINDIR and many other useful macros.
include(GNUInstallDirs)
include(CTest)

# Control where libraries and executables are placed during the build.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")

# Build Position Independent Code (PIC).
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Disable C and C++ compiler extensions.
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Export compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Threads REQUIRED)

# endian support
include(TestBigEndian)
TEST_BIG_ENDIAN(IS_BIG_ENDIAN)

# Build debug-configuration by default
if (NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Debug)
endif ()

# Some options for the compiler
add_compile_options(
   $<$<CONFIG:DEBUG>:-g>
   $<$<CONFIG:RELEASE>:-O3>
   -Wall
)

# Strip everything
add_link_options($<$<CONFIG:RELEASE>:-s>)

### Compile- and install-related commands.
add_subdirectory(submodules)

### Compile- and install-related commands.
add_subdirectory(src)

# add examples
if (BUILD_EXAMPLES)
   add_subdirectory(examples)
endif ()

# Add tests
if (BUILD_TESTING)
   add_subdirectory(test)
endif ()

# Add targets related to doxygen documentation generation
if (BUILD_DOC)
   add_subdirectory(doc)
endif ()
