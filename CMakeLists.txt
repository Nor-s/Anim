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

add_subdirectory(external/glad/)
add_subdirectory(external/imgui/)
if(APPLE)
  add_subdirectory(external/assimp)
endif()
if(WIN)
  find_package(assimp REQUIRED FATAL_ERROR)
endif()
add_subdirectory(external/nfd)
add_library(STB_IMAGE "external/stb/stb_image.cpp")
find_package(glfw3 REQUIRED FATAL_ERROR)
find_package(glm REQUIRED FATAL_ERROR)
add_subdirectory(src)