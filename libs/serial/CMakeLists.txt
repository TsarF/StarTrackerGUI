cmake_minimum_required(VERSION 3.20)
project(serial)

if(APPLE)
    find_package(IOKit REQUIRED)
    find_package(Foundation REQUIRED)
endif()

find_package(Threads REQUIRED)

## Sources
set(serial_SRCS
    src/serial.cc
    include/serial/serial.h
    include/serial/v8stdint.h
)
if(APPLE)
    # If OSX
    list(APPEND serial_SRCS src/impl/unix.cc)
    list(APPEND serial_SRCS src/impl/list_ports/list_ports_osx.cc)
elseif(UNIX)
    # If unix
    list(APPEND serial_SRCS src/impl/unix.cc)
    list(APPEND serial_SRCS src/impl/list_ports/list_ports_linux.cc)
else()
    # If windows
    list(APPEND serial_SRCS src/impl/win.cc)
    list(APPEND serial_SRCS src/impl/list_ports/list_ports_win.cc)
endif()

## Add serial library
add_library(serial ${serial_SRCS})
if(APPLE)
    target_link_libraries(serial PRIVATE IOKit Foundation)
elseif(UNIX)
    target_link_libraries(serial PRIVATE rt)
else()
    target_link_libraries(serial PRIVATE setupapi)
endif()

target_link_libraries(serial PRIVATE Threads::Threads)

target_include_directories(serial PRIVATE include)