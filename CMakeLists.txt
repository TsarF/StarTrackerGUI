cmake_minimum_required(VERSION 3.20)

set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

project(StarTrackerGUI)

set(SOURCES
src/common.cpp
src/mainWindow.cpp
src/settingsWindow.cpp
src/utils.cpp
src/settings.cpp
src/tashimiTaki.cpp
src/astroUtils.cpp
src/protocol.cpp
)

set(HEADERS
inc/platform.h
inc/utils.h
)

find_package(wxWidgets CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)

add_subdirectory(libs/serial)

add_executable(StarTrackerGUI WIN32 src/main.cpp ${SOURCES} ${HEADERS})
add_executable(testConversion src/testConversion.cpp ${SOURCES} ${HEADERS})


target_include_directories(StarTrackerGUI PRIVATE inc)
target_include_directories(testConversion PRIVATE inc)

target_include_directories(StarTrackerGUI PRIVATE libs/serial/include)

include_directories(${Boost_INCLUDE_DIR})

target_link_libraries(StarTrackerGUI PRIVATE wx::core wx::base)
target_link_libraries(StarTrackerGUI PRIVATE serial)
target_link_libraries(StarTrackerGUI PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(StarTrackerGUI PRIVATE Eigen3::Eigen)

target_link_libraries(testConversion PRIVATE wx::core wx::base)
target_link_libraries(StarTrackerGUI PRIVATE serial)
target_link_libraries(testConversion PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(testConversion PRIVATE Eigen3::Eigen)

add_compile_definitions(SOURCE_DIR="${CMAKE_SOURCE_DIR}")

if (WIN32)
    add_compile_definitions(WIN32="")

    target_link_libraries(StarTrackerGUI PRIVATE OneCore)
    target_link_libraries(testConversion PRIVATE OneCore)
endif (WIN32)

if (UNIX)
    add_compile_definitions(UNIX="")
endif (UNIX)