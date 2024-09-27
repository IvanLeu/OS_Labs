cmake_minimum_required(VERSION 3.26)

project(os_lab_1)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED)

add_subdirectory("src")
add_subdirectory("src/child")