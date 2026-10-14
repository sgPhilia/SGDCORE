cmake_minimum_required(VERSION 3.15)
project(SGDCOREInstalledConsumer LANGUAGES CXX)

find_package(SGDCORE 0.1 CONFIG REQUIRED)

add_executable(sgdcore_package_consumer main.cpp)
target_link_libraries(sgdcore_package_consumer PRIVATE SGDCORE::sgdcore)

enable_testing()
add_test(NAME InstalledPackageConsumer COMMAND sgdcore_package_consumer)
