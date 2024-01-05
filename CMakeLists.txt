cmake_minimum_required(VERSION 3.20)

set(CMAKE_TOOLCHAIN_FILE "C:/Users/timtc/Documents/Repos/vcpkg/scripts/buildsystems/vcpkg.cmake")

project(StarTrackerGUI)

set(SOURCES
src/common.cpp
src/main.cpp
src/mainWindow.cpp
src/settingsWindow.cpp
src/utils.cpp
src/serial.cpp
src/settings.cpp
src/tashimiTaki.cpp
src/astroUtils.cpp
)

set(HEADERS
inc/platform.h
)

find_package(wxWidgets CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Boost 1.83.0 REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)

add_executable(StarTrackerGUI WIN32 ${SOURCES} ${HEADERS})

target_include_directories(StarTrackerGUI PRIVATE inc)
include_directories(${Boost_INCLUDE_DIR})

target_link_libraries(StarTrackerGUI PRIVATE wx::core wx::base)
target_link_libraries(StarTrackerGUI PRIVATE ${Boost_LIBRARIES})
target_link_libraries(StarTrackerGUI PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(StarTrackerGUI PRIVATE Eigen3::Eigen)

add_compile_definitions(SOURCE_DIR="${CMAKE_SOURCE_DIR}")

if (WIN32)
    add_compile_definitions(WIN32="")

    target_link_libraries(StarTrackerGUI PRIVATE OneCore)
endif (WIN32)

if (UNIX)
    add_compile_definitions(UNIX="")
endif (UNIX)