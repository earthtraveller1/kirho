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
target_include_directories(kirho INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>" "$<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>")

# Add the tests, but only if we are not included in another project.
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    add_subdirectory(tests)
endif()

# Now, we get to the fun part. We need to export the targets, but only if we are not 
# not included in another subdirectory

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    # Install the library, I presume.
    install(
        TARGETS kirho EXPORT kirhoTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
    )

    # Install the header file.
    install(
        FILES include/kirho/kirho.hpp
        DESTINATION include/kirho
        COMPONENT Devel
    )

    # Now, we need to write the version configuration file.
    include(CMakePackageConfigHelpers)
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/kirhoConfigVersion.cmake"
        VERSION 0.1
        COMPATIBILITY AnyNewerVersion
    )

    # Then, we need to export the kirho target
    export(
        EXPORT kirhoTargets
        FILE "${CMAKE_CURRENT_BINARY_DIR}/kirhoTargets.cmake"
        NAMESPACE kirho::
    )

    # Now, after we have exported the target, we need to configure it's 
    # configuration file. Yeah, if you are wondering if I have any idea 
    # what is going on, the truth is that I do not, I am just following
    # CMake's guide on what I should do.
    configure_file(
        cmake/kirhoConfig.cmake
        "${CMAKE_CURRENT_BINARY_DIR}/kirhoConfig.cmake"
        COPYONLY
    )
    
    # This is the location in which we will install all of the CMake package
    # configuration files, relative to the install prefix.
    set(KIRHO_CONFIG_PACKAGE_LOCATION share/cmake/kirho)

    # Now, we need to install the targets file.
    install(
        EXPORT kirhoTargets
        FILE kirhoTargets.cmake
        NAMESPACE kirho::
        DESTINATION ${KIRHO_CONFIG_PACKAGE_LOCATION}
    )

    # And finally, install the configuration and the version file.
    install(
        FILES 
            cmake/kirhoConfig.cmake
            "${CMAKE_CURRENT_BINARY_DIR}/kirhoConfigVersion.cmake"
        DESTINATION
            ${KIRHO_CONFIG_PACKAGE_LOCATION}
        COMPONENT
            Devel
    )
endif()
