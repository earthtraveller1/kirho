# We need features from CMake 3.13 and below.
cmake_minimum_required(VERSION 3.13)

# Include the required CMake modules. We will be installing and exporting packages
# in this case, so we will need the install directories and the package configuration
# helpers
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# We only call project if we are not a subdirectory in another project.
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    project(kirho)
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

# Our library currently does not have any source files, so we will make it an interface
# library.
add_library(kirho INTERFACE)

# Add the include directories so that when an application links with this library, 
# it will automatically get the include directories.
target_include_directories(
    kirho PUBLIC 
    "$<BUILD_INTERACE:${CMAKE_SOURCE_DIR}/include>" 
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

# This is where the fun part begins. First, we need to export the kirho target by
# using the install command, and specifying where everything is going to go.
install(
    TARGETS kirho
    EXPORT kirhoTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Next, we need to install the header file to the correct location. Currently, we
# only have one, so this should be good enough.
install(
    FILES include/kirho/kirho.hpp
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT Devel
)

# Create the package version file. Pretty self-explanatory. The package version file
# is one of the files that are needed for CMake to find our project.
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/kirho/kirhoConfigVersion.cmake"
    VERSION "0.1"
    COMPATIBILITY AnyNewerVersion
)

# Now, we export the exported target to a file. Yes, confusing how we need to export
# something twice, but this is CMake, so nothing is really simple, so shut the fuck
# up.
export(
    EXPORT kirhoTargets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/kirho/kirhoTargets.cmake"
    NAMESPACE kirho::
)

# Now, we need to configure the package configuration file. In other words, we need
# to copy it to the build directory. That's why we marked it as copy only.
configure_file(
    cmake/kirhoConfig.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/kirho/kirhoStatsConfig.cmake"
    COPYONLY
)

# This is where the package configuration files will be installed, relative to the 
# install prefix, that is.
set(KIRHO_CONFIG_PACKAGE_DIR share/kirho)

# Now, we need to install the exported target file to the installation prefix place.
# This is fairly simple, to be honest with you.
install(
    EXPORT kirhoTargets
    FILE kirhoTargets.cmake
    NAMESPACE kirho::
    DESTINATION ${KIRHO_CONFIG_PACKAGE_DIR}
)

# Now we need to install the configuration file and the version file. Again, this should
# also be fairly simple.
install(
    FILES
        cmake/kirhoConfig.cmake
        "${CMAKE_CURRENT_BINARY_DIR}/kirho/kirhoConfigVersion.cmake"
    DESTINATION
        ${KIRHO_CONFIG_PACKAGE_DIR}
    COMPONENT
        Devel
)

# Now, we need to install the license file. This is just something that's required by vcpkg,
# not something that is mandatory in general.
install(
    FILES "${CMAKE_SOURCE_DIR}/LICENSE"
    DESTINATION ${KIRHO_CONFIG_PACKAGE_DIR}
    RENAME copyright
)
