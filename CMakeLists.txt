cmake_minimum_required(VERSION 3.20)

#set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake")
set(CMAKE_TOOLCHAIN_FILE "~/Documents/repos/vcpkg/scripts/buildsystems/vcpkg.cmake")

project(StarTrackerGUI)

set(SOURCES
src/common.cpp
src/mainWindow.cpp
src/settingsWindow.cpp
src/utils.cpp
src/settings.cpp
src/toshimiTaki.cpp
src/astroUtils.cpp
src/protocol.cpp
src/firmwareUpdateWindow.cpp
)

set(HEADERS
inc/protocol.h
inc/astroUtils.h
inc/common.h
inc/firmwareUpdateWindow.h
inc/mainWindow.h
inc/settings.h
inc/settingsWindow.h
inc/toshimiTaki.h
inc/platform.h
inc/utils.h
)

find_package(wxWidgets CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)

add_subdirectory(libs/serial)

add_executable(StarTrackerGUI WIN32 src/main.cpp ${SOURCES} ${HEADERS})


target_include_directories(StarTrackerGUI PRIVATE inc)

target_include_directories(StarTrackerGUI PRIVATE libs/serial/include)

target_link_libraries(StarTrackerGUI PRIVATE wx::core wx::base)
target_link_libraries(StarTrackerGUI PRIVATE serial)
target_link_libraries(StarTrackerGUI PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(StarTrackerGUI PRIVATE Eigen3::Eigen)

add_compile_definitions(SOURCE_DIR="${CMAKE_SOURCE_DIR}")

if (WIN32)
    add_compile_definitions(WIN32="")

    target_link_libraries(StarTrackerGUI PRIVATE OneCore)
endif (WIN32)

if(APPLE)
    add_compile_definitions(MACOS="")
endif(APPLE)

if (UNIX)
    add_compile_definitions(UNIX="")
endif (UNIX)

set_property(TARGET StarTrackerGUI PROPERTY C_STANDARD 17)