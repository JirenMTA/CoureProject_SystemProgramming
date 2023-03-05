cmake_minimum_required(VERSION 3.20)
set (CMAKE_TOOLCHAIN_FILE "/usr/local/vcpkg/scripts/buildsystems/vcpkg.cmake")

project(Client)
set(CMAKE_CXX_STANDARD 17)
set(CURL_DIR "/usr/local/vcpkg/installed/x64-windows/share/curl")
find_package(nlohmann_json CONFIG REQUIRED)
find_library(libconfig libconfig)

add_executable(Client "Client.cpp")
target_link_libraries(Client PRIVATE config)
target_link_libraries(Client PRIVATE nlohmann_json::nlohmann_json)
