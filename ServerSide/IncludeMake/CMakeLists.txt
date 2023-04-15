cmake_minimum_required(VERSION 3.22)

set(CMAKE_TOOLCHAIN_FILE "/usr/local/vcpkg/scripts/buildsystems/vcpkg.cmake")

project(StorageLib VERSION 1.0.0 DESCRIPTION "Lib for access to Security Storage")

set(CMAKE_CXX_STANDARD 17)

# Find required packages
find_package(nlohmann_json CONFIG REQUIRED)
find_library(libconfig libconfig)

# Find source files
file(GLOB SOURCES src/*.cpp src/*.h)

# Include header files
include_directories(include)

# Create shared library
add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PRIVATE
    "/usr/local/vcpkg/installed/x64-linux/include/nlohmann"
)

# Set target properties
file(GLOB_RECURSE EDV_INCLUDE_FILES "include/*.h*")

set_target_properties(${PROJECT_NAME}
    PROPERTIES PUBLIC_HEADER "${EDV_INCLUDE_FILES}"
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE config)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)

# Install targets
include(GNUInstallDirs)
install(TARGETS StorageLib
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/StorageLib
)
configure_file(StorageLib.pc.in StorageLib.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/StorageLib.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)
