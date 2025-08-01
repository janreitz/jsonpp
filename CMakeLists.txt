cmake_minimum_required(VERSION 3.20)
project(jsonpp 
    VERSION 0.1.0
    DESCRIPTION "A header-only JSON pull parser with schema validation"
    LANGUAGES CXX)

# Set C++20 as minimum standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Create header-only interface library
add_library(jsonpp INTERFACE)
add_library(jsonpp::jsonpp ALIAS jsonpp)

# Set include directories
target_include_directories(jsonpp INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Compiler-specific optimizations and warnings
target_compile_features(jsonpp INTERFACE cxx_std_20)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(jsonpp INTERFACE
        $<BUILD_INTERFACE:
            -Wall -Wextra -Wpedantic
            -Wconversion -Wsign-conversion
            -Wno-unused-parameter
        >
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(jsonpp INTERFACE
        $<BUILD_INTERFACE:/W4 /permissive->
    )
endif()

# Enable testing if this is the main project
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
    enable_testing()
    
    if(BUILD_TESTING)
        add_subdirectory(tests)
    endif()
endif()

# Installation configuration
include(GNUInstallDirs)
install(TARGETS jsonpp
    EXPORT jsonpp-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT jsonpp-targets
    FILE jsonpp-targets.cmake
    NAMESPACE jsonpp::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/jsonpp
)

# Create config file
include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/jsonpp-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/jsonpp-config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/jsonpp
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/jsonpp-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/jsonpp-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/jsonpp-config-version.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/jsonpp
)