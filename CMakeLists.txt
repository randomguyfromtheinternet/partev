cmake_minimum_required(VERSION 3.10)
project(particlesim LANGUAGES CXX)

# C++17 und Compiler-Flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2")

# Quellcode liegt in ./src
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
file(GLOB SOURCES ${SOURCE_DIR}/*.cpp)

# Ausgabeverzeichnis für Binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Executable erstellen
add_executable(particlesim ${SOURCES})

# SFML einbinden
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
target_link_libraries(particlesim sfml-graphics sfml-window sfml-system)

# Include-Pfade setzen (optional, falls du Header in ./src nutzt)
target_include_directories(particlesim PRIVATE ${SOURCE_DIR})
