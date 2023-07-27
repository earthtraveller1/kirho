# We need features from CMake 3.13 and below.
cmake_minimum_required(VERSION 3.13)

# We only call project if we are not a subdirectory in another project.
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    project(kirho CXX)
endif()

# We want to use C++20.
set(CMAKE_CXX_STANDARD 20)

# Enable the greatest resonable warning level, and force us to handle them by making
# them errors.
if (MSVC)
    add_compile_options(/W4 /Werror)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# CMake sometimes doesn't insert enough debug information, so we need to force it 
# to do so, just to be sure.
if (CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT MSVC)
    add_compile_options(-g3)
    add_link_options(-g3)
endif()

# We are currently only a headers-only library, so we are only going to create an interface
# library.
add_library(kirho INTERFACE)
target_include_directories(kirho INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")

message(STATUS "The include directory is located at: ${CMAKE_CURRENT_SOURCE_DIR}/include")

# Add the tests, but only if we are not included in another project.
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    add_subdirectory(tests)
endif()
