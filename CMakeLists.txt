cmake_minimum_required(VERSION 3.15)
project(Wizz_Air CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# CMake configuration for Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# --- Compiler Configuration (MSYS2) ---
if (WIN32)
    set(CMAKE_CXX_COMPILER "C:/msys64/mingw64/bin/g++.exe")
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

# === Qt ===
set(CMAKE_PREFIX_PATH "C:/Qt/6.8.3/mingw_64")
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# === Program ===
file(GLOB SOURCES "src/*.cpp")
add_executable(Wizz_Air ${SOURCES})  # WIN32 to match as a graphic app
include_directories(include)

if (WIN32)
    target_link_libraries(Wizz_Air PRIVATE ws2_32 wsock32 Qt6::Widgets)
endif ()

# Enable detailed build output
set(CMAKE_VERBOSE_MAKEFILE ON)
