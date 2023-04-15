cmake_minimum_required(VERSION 3.20)

project(ListeningDaemon)
set(CMAKE_CXX_STANDARD 17)

include("/usr/local/vcpkg/scripts/buildsystems/vcpkg.cmake")
find_package(PkgConfig REQUIRED)
pkg_check_modules(StorageLib REQUIRED IMPORTED_TARGET StorageLib)
find_package(nlohmann_json CONFIG REQUIRED)
find_library(libconfig libconfig)
include_directories(include)
add_executable( ListeningDaemon
		src/common.cpp
		src/functions_of_types.cpp
		src/ListeningDaemon.cpp	
)
target_link_libraries(ListeningDaemon PRIVATE PkgConfig::StorageLib nlohmann_json::nlohmann_json config)
