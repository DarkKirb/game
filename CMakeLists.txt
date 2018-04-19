cmake_minimum_required(VERSION 3.6)
project(fangame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_BUILD_TYPE Debug)
set(APP_VERSION_MAJOR 0)
set(APP_VERSION_MINOR 1)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(APP_TARGET game)

find_package(PythonInterp 3 REQUIRED)

add_subdirectory(assets)
