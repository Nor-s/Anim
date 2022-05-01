cmake_minimum_required(VERSION 3.21)

message("Using CMake version ${CMAKE_VERSION}")
if(APPLE)
  find_program(CCACHE_PROGRAM ccache)
  if(CCACHE_PROGRAM)
      set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
  endif()
endif()

project(
  n-glcpp
  LANGUAGES CXX
  VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)

if(APPLE)
  get_property(RULE_LAUNCH_COMPILE GLOBAL PROPERTY RULE_LAUNCH_COMPILE)
  if(RULE_LAUNCH_COMPILE AND CMAKE_GENERATOR STREQUAL "Xcode")
      # Set up wrapper scripts
      configure_file(launch-c.in launch-c)
      configure_file(launch-cxx.in launch-cxx)
      execute_process(COMMAND chmod a+rx
                               "${CMAKE_BINARY_DIR}/launch-c"
                               "${CMAKE_BINARY_DIR}/launch-cxx"
      )

      # Set Xcode project attributes to route compilation and linking
      # through our scripts
      set(CMAKE_XCODE_ATTRIBUTE_CC         "${CMAKE_BINARY_DIR}/launch-c")
      set(CMAKE_XCODE_ATTRIBUTE_CXX        "${CMAKE_BINARY_DIR}/launch-cxx")
      set(CMAKE_XCODE_ATTRIBUTE_LD         "${CMAKE_BINARY_DIR}/launch-c")
      set(CMAKE_XCODE_ATTRIBUTE_LDPLUSPLUS "${CMAKE_BINARY_DIR}/launch-cxx")
  endif()
endif()

# set(CMAKE_BUILD_TYPE Debug)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

message(STATUS "Compiler")
message(STATUS " - ID       \t: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " - Version  \t: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS " - Path     \t: ${CMAKE_CXX_COMPILER}")

# option(BUILD_SHARED_LIBS ON)

option(JSONCPP_WITH_TESTS OFF)
option(GLFW_BUILD_DOCS OFF)
option(ASSIMP_BUILD_TESTS OFF)
option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)

add_subdirectory(external/glad/)
if(APPLE)
  add_subdirectory(external/imgui/)
  add_subdirectory(external/assimp/)
  add_subdirectory(external/glfw/)
endif()
if(WIN32)
  add_subdirectory(external/glfw/)
  add_subdirectory(external/imgui/)
  add_subdirectory(external/assimp/)
endif()
add_subdirectory(external/nfd)
add_subdirectory(external/jsoncpp)
add_subdirectory(external/glm/)

add_library(STB_IMAGE "external/stb/stb_image.cpp")

add_subdirectory(src)
add_dependencies (imgui glfw)