cmake_minimum_required(VERSION 3.21)

message("Using CMake version ${CMAKE_VERSION}")

project(
  n-glcpp
  LANGUAGES CXX
  VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE Debug)

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


message(STATUS "Compiler")
message(STATUS " - ID       \t: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " - Version  \t: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS " - Path     \t: ${CMAKE_CXX_COMPILER}")

option(BUILD_SHARED_LIBS ON)
option(GLFW_BUILD_DOCS OFF)
add_subdirectory(external/glfw/)
if(APPLE)
  add_subdirectory(external/assimp)
endif()
if(WIN32)
  option(ASSIMP_BUILD_TESTS  OFF)
  option(ASSIMP_BUILD_ASSIMP_TOOLS  OFF)
  add_subdirectory(external/assimp)
  # find_package(assimp REQUIRED FATAL_ERROR)
endif()
add_subdirectory(external/nfd)
# add_subdirectory(external/jsoncpp)
add_library(STB_IMAGE "external/stb/stb_image.cpp")
add_subdirectory(external/glm/)

# find_package(glfw3 REQUIRED FATAL_ERROR)
# find_package(glm REQUIRED FATAL_ERROR)
add_subdirectory(external/glad/)
add_subdirectory(external/imgui/)
add_subdirectory(src)