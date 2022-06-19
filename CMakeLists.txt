##
# CMakefile.txt: CMakeLists.txt
# Project: Template
# Date: 2021-12-16
# Author: <YOUR NAME>
# Notes:
##

cmake_minimum_required(VERSION 3.5)

# Set the PROJECT_NAME, PROJECT_VERSION as well as other variable
project(cppsl
        DESCRIPTION "C++ support library"
        LANGUAGES CXX C
        VERSION 1.0.0.0)

# Defines the CMAKE_INSTALL_LIBDIR, CMAKE_INSTALL_BINDIR and many other useful macros.
# See https://cmake.org/cmake/help/latest/module/GNUInstallDirs.html
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(CMakeDependentOption)
include(CTest)

# Control where libraries and executables are placed during the build.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")

# Under MSVC, we set CMAKE_DEBUG_POSTFIX to "d" to add a trailing "d" to library
# built in debug mode.
if (MSVC)
    set(CMAKE_DEBUG_POSTFIX "d")
endif ()

# Build Position Independent Code (PIC).
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Disable C and C++ compiler extensions.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add packages
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

### Options
option(BUILD_SHARED_LIBS "Build libraries as shared as opposed to static" OFF)
option(BUILD_TESTING "Create tests using CMake" OFF)
option(BUILD_EXAMPLES "Build examples using CMake" OFF)

# Build test related commands?
if (BUILD_TESTING)
    enable_testing()
endif ()

# Enable RPATH support for installed binaries and libraries
include(AddInstallRPATHSupport)
add_install_rpath_support(BIN_DIRS "${CMAKE_INSTALL_FULL_BINDIR}"
        LIB_DIRS "${CMAKE_INSTALL_FULL_LIBDIR}"
        INSTALL_NAME_DIR "${CMAKE_INSTALL_FULL_LIBDIR}"
        USE_LINK_PATH)

# Encourage user to specify a build type (e.g. Release, Debug, etc.), otherwise set it to Release.
if (NOT CMAKE_CONFIGURATION_TYPES)
    if (NOT CMAKE_BUILD_TYPE)
        message(STATUS "Setting build type to 'Release' as none was specified.")
        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY VALUE "Release")
    endif ()
endif ()

set(cppsl_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

### Compile external.
set(fmt_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/external/fmt/fmt)
set(spdlog_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/external/spdlog/include)

add_subdirectory(external)

### Compile- and install-related commands.
add_subdirectory(src)

# Create and install CMake configuration files for your project that are
# necessary to for other projects to call find_package(cppsl).
include(InstallBasicPackageFiles)
install_basic_package_files(${PROJECT_NAME}
        VERSION ${${PROJECT_NAME}_VERSION}
        COMPATIBILITY AnyNewerVersion
        VARS_PREFIX ${PROJECT_NAME}
        NO_CHECK_REQUIRED_COMPONENTS_MACRO)
# Add the uninstall target
include(AddUninstallTarget)

# Add integration tests (unit tests for each library should be in each subdirectory).
if (BUILD_TESTING)
    add_subdirectory(test)
endif ()

# Add examples?
if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif ()

# Add targets related to doxygen documentation generation
add_subdirectory(doc)
