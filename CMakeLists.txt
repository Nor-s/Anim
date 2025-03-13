cmake_minimum_required(VERSION 3.21)
# include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

message("Using CMake version ${CMAKE_VERSION}")

# if($ENV{COMPILE_FOR_WASM})
# 	set(COMPILE_DEFINITIONS _PLATFORM_WASM)
# 	set(CMAKE_TOOLCHAIN_FILE $ENV{EMSDK_HOME}/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake)
# 	set(CMAKE_EXECUTABLE_SUFFIX ".html")
# endif()


# for CCACHE
if(APPLE)
  find_program(CCACHE_PROGRAM ccache)
  if(CCACHE_PROGRAM)
      set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
  endif()
endif()

project(
  Anim
  LANGUAGES CXX
  VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 20)

# for ninja(unix)
if (UNIX AND NOT APPLE)
    set(CMAKE_MAKE_PROGRAM /usr/bin/ninja CACHE FILEPATH "")
endif()

message("CMAKE_RUNTIME_OUTPUT_DIRECTORY : ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message("CMAKE_BINARY_DIR : ${CMAKE_BINARY_DIR}")


# for output directory
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(COPY resources DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY imgui.ini DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY LICENSE DESTINATION ${CMAKE_BINARY_DIR}/bin)

message("Compiler")
message(STATUS " - ID       \t: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " - Version  \t: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS " - Path     \t: ${CMAKE_CXX_COMPILER}")

# for external option
option(JSONCPP_WITH_TESTS OFF)
option(GLFW_BUILD_DOCS OFF)
option(ASSIMP_BUILD_ZLIB ON)
option(ASSIMP_BUILD_TESTS OFF)
option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
option(ASSIMP_RAPIDJSON_NO_MEMBER_ITERATOR OFF)
set(ASSIMP_RAPIDJSON_NO_MEMBER_ITERATOR OFF)
set(ASSIMP_BUILD_ZLIB ON)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
set(BUILD_SHARED_LIBS OFF)



find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

include_directories(SYSTEM external external/)
add_compile_options(-Wno-nontrivial-memcall)



# add external libraries
add_subdirectory(external/glad/)
add_library(STB_IMAGE STATIC "external/stb/stb_image.cpp")
# add_subdirectory(external/glfw/)
add_subdirectory(external/imgui/)
add_subdirectory(external/assimp/)
message("------------glfw")
add_subdirectory(external/jsoncpp)
add_subdirectory(external/glm/)

# add_subdirectory(src)
# add_dependencies (imgui glfw)
# add_dependencies(STB_IMAGE imgui)
# add_dependencies(glcpp assimp)
# add_subdirectory(playground/export_test)


add_subdirectory(test/external_wasm)
add_subdirectory(test/anim_wasm)