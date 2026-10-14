cmake_minimum_required(VERSION 3.15)
project(SGDCORE VERSION 0.1.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(sgdcore src/tape.cpp)
target_include_directories(sgdcore PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(sgdcore PUBLIC cxx_std_20)
add_library(SGDCORE::sgdcore ALIAS sgdcore)

set(SGDCORE_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/SGDCORE")
install(TARGETS sgdcore
    EXPORT SGDCORETargets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)
install(DIRECTORY include/ DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
install(EXPORT SGDCORETargets
    FILE SGDCORETargets.cmake
    NAMESPACE SGDCORE::
    DESTINATION "${SGDCORE_INSTALL_CMAKEDIR}"
)

configure_package_config_file(
    cmake/SGDCOREConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/SGDCOREConfig.cmake"
    INSTALL_DESTINATION "${SGDCORE_INSTALL_CMAKEDIR}"
)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/SGDCOREConfigVersion.cmake"
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY SameMajorVersion
)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/SGDCOREConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/SGDCOREConfigVersion.cmake"
    DESTINATION "${SGDCORE_INSTALL_CMAKEDIR}"
)

option(SGDCORE_BUILD_TESTS "Build tests (fetches pinned GoogleTest)" OFF)
option(SGDCORE_BUILD_BENCHMARKS "Build benchmarks (fetches pinned Google Benchmark)" OFF)
option(SGDCORE_BUILD_EXAMPLES "Build examples" OFF)

include(FetchContent)

if(SGDCORE_BUILD_TESTS)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG f8d7d77c06936315286eb55f8de22cd23c188571
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    enable_testing()
    add_subdirectory(tests)
endif()

if(SGDCORE_BUILD_BENCHMARKS)
    FetchContent_Declare(
        googlebenchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG 344117638c8ff7e239044fd0fa7085839fc03021
    )
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googlebenchmark)
    add_subdirectory(benchmarks)
endif()

if(SGDCORE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
