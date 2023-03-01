cmake_minimum_required(VERSION 3.9)
project(ListeningDaemon)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_TOOLCHAIN_FILE "/usr/local/vcpkg/scripts/buildsystems/vcpkg.cmake")

include("/usr/local/vcpkg/scripts/buildsystems/vcpkg.cmake")
find_package(nlohmann_json CONFIG REQUIRED)
add_executable(ListeningDaemon "ListeningDaemon.cpp")
target_link_libraries(ListeningDaemon PRIVATE nlohmann_json::nlohmann_json)

find_library(libconfig libconfig)
target_link_libraries(ListeningDaemon PRIVATE config)