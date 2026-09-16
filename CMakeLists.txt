cmake_minimum_required(VERSION 3.14)
project(SGDCORE VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(sgdcore src/tape.cpp)
target_include_directories(sgdcore PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(sgdcore PUBLIC cxx_std_20)

option(SGDCORE_BUILD_TESTS "Build tests" ON)
option(SGDCORE_BUILD_BENCHMARKS "Build benchmarks" ON)
option(SGDCORE_BUILD_EXAMPLES "Build examples" ON)

include(FetchContent)

if(SGDCORE_BUILD_TESTS)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    enable_testing()
    add_subdirectory(tests)
endif()

if(SGDCORE_BUILD_BENCHMARKS)
    FetchContent_Declare(
        googlebenchmark
        URL https://github.com/google/benchmark/archive/refs/tags/v1.8.3.zip
    )
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googlebenchmark)
    add_subdirectory(benchmarks)
endif()

if(SGDCORE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()