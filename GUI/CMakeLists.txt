#cmake_minimum_required(VERSION 3.23)
#project(untitled)
#
#set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTORCC ON)
#set(CMAKE_AUTOUIC ON)
#
#
#find_package(Qt5 COMPONENTS
#        Core
#        Gui
#        Widgets
#        REQUIRED)
#
#add_executable( main.cpp MainWindow.cpp MainWindow.h WindowRead.cpp WindowRead.h WindowGetRight.cpp WindowGetRight.h WindowGrant.cpp WindowGrant.h WindowListStorage.cpp WindowListStorage.h WindowRevoke.cpp WindowRevoke.h WindowLogin.cpp WindowLogin.h WindowDelete.cpp WindowDelete.h)
#target_link_libraries(untitled
#        Qt5::Core
#        Qt5::Gui
#        Qt5::Widgets
#        )


cmake_minimum_required(VERSION 3.20)
project(untitled)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)
include("/home/lisa/Desktop/New Folder/CoureProject_SystemProgramming/dependency/vcpkg/scripts/buildsystems/vcpkg.cmake")
find_package(PkgConfig REQUIRED)
pkg_check_modules(StorageLib REQUIRED IMPORTED_TARGET StorageLib)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Curses REQUIRED)

add_executable(untitled main.cpp MainWindow.cpp MainWindow.h WindowRead.cpp WindowRead.h WindowGetRight.cpp WindowGetRight.h WindowGrant.cpp WindowGrant.h WindowListStorage.cpp WindowListStorage.h WindowRevoke.cpp WindowRevoke.h WindowLogin.cpp WindowLogin.h WindowDelete.cpp WindowDelete.h WindowBanUser.cpp WindowBanUser.h WindowPasswdFile.cpp WindowPasswdFile.h)
target_include_directories(untitled PUBLIC "${CMAKE_SOURCE_DIR}/include/security")
target_link_libraries(untitled
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        PkgConfig::StorageLib
        nlohmann_json::nlohmann_json
        )
target_link_libraries(untitled pam pam_misc)
target_link_libraries(untitled crypt cap)
