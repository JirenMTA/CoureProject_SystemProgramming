cmake_minimum_required(VERSION 3.20)

project(Client)
set(CMAKE_CXX_STANDARD 17)

include("/usr/local/vcpkg/scripts/buildsystems/vcpkg.cmake")
find_package(PkgConfig REQUIRED)
pkg_check_modules(StorageLib REQUIRED IMPORTED_TARGET StorageLib)
find_package(nlohmann_json CONFIG REQUIRED)

add_executable(Client "Client.cpp")
target_link_libraries(Client PRIVATE PkgConfig::StorageLib nlohmann_json::nlohmann_json)
