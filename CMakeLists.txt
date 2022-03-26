cmake_minimum_required(VERSION 3.21)

message("Using CMake version ${CMAKE_VERSION}")

project(
  n-glcpp
  LANGUAGES CXX
  VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)

message(STATUS "Compiler")
message(STATUS " - ID       \t: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " - Version  \t: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS " - Path     \t: ${CMAKE_CXX_COMPILER}")

set(BUILD_SHARED_LIBS OFF)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
add_subdirectory(external/glad/)
add_subdirectory(external/imgui/)
add_subdirectory(external/assimp)

add_subdirectory(external/nfd)
add_library(STB_IMAGE "external/stb/stb_image.cpp")

add_subdirectory(src)