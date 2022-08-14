cmake_minimum_required(VERSION 3.21)
# include(ExternalProject)
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

# cmake_minimum_required(VERSION 3.12...3.15)
message("Using CMake version ${CMAKE_VERSION}")
if(APPLE)
  find_program(CCACHE_PROGRAM ccache)
  if(CCACHE_PROGRAM)
      set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
  endif()
endif()

project(
  Anim
  LANGUAGES CXX
  VERSION 0.0.1)
set(CMAKE_CXX_STANDARD 17)

if (UNIX AND NOT APPLE)
    set(CMAKE_MAKE_PROGRAM /usr/bin/ninja CACHE FILEPATH "")
endif()

message("CMAKE_RUNTIME_OUTPUT_DIRECTORY : ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message("CMAKE_BINARY_DIR : ${CMAKE_BINARY_DIR}")

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


set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(COPY resources DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY py_module DESTINATION ${CMAKE_BINARY_DIR}/bin)
file(COPY imgui.ini DESTINATION ${CMAKE_BINARY_DIR}/bin)
message("==========================================================")
if (APPLE)
#   execute_process(COMMAND sh "install_mp2mm.sh"
#     WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
else()
  message("Build python")
  execute_process(COMMAND cmd /C ${CMAKE_CURRENT_SOURCE_DIR}/scripts/build_python.bat
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
endif()
if (NOT EXISTS ${CMAKE_BINARY_DIR}/bin/python/python.exe)
  file(COPY ${CMAKE_CURRENT_LIST_DIR}/python DESTINATION ${CMAKE_BINARY_DIR}/bin)
  file(COPY ${CMAKE_CURRENT_LIST_DIR}/python/python310.dll DESTINATION ${CMAKE_BINARY_DIR}/bin)
  file(COPY ${CMAKE_CURRENT_LIST_DIR}/python/python3.dll DESTINATION ${CMAKE_BINARY_DIR}/bin)
endif()

message("==========================================================")

message("Compiler")
message(STATUS " - ID       \t: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS " - Version  \t: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS " - Path     \t: ${CMAKE_CXX_COMPILER}")

# option(BUILD_SHARED_LIBS OFF)
option(JSONCPP_WITH_TESTS OFF)
option(GLFW_BUILD_DOCS OFF)
option(ASSIMP_BUILD_ZLIB ON)
option(ASSIMP_BUILD_TESTS OFF)
option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
if(WIN32)
  option(ASSIMP_RAPIDJSON_NO_MEMBER_ITERATOR OFF)
endif()
set(ASSIMP_BUILD_ZLIB ON)
message("GLAD")
add_subdirectory(external/glad/)
message("STB")
add_library(STB_IMAGE "external/stb/stb_image.cpp")



if(WIN32)
  message("GLFW")
  add_subdirectory(external/glfw/)
  message("IMGUI")
  add_subdirectory(external/imgui/)
  message("ASSIMP")
  add_subdirectory(external/assimp/)
else()
  add_subdirectory(external/imgui/)
  add_subdirectory(external/assimp/)
  add_subdirectory(external/glfw/)
endif()

# set(ASSIMP_CMAKE_OPTION -DJSONCPP_WITH_TESTS=OFF -DGLFW_BUILD_DOCS=OFF -DASSIMP_BUILD_ZLIB=ON -DASSIMP_BUILD_TESTS=OFF -DASSIMP_BUILD_ASSIMP_TOOLS=OFF)
# if(WIN32)
# set(ASSIMP_CMAKE_OPTION -DJSONCPP_WITH_TESTS=OFF -DGLFW_BUILD_DOCS=OFF -DASSIMP_BUILD_ZLIB=ON -DASSIMP_BUILD_TESTS=OFF -DASSIMP_BUILD_ASSIMP_TOOLS=OFF -DASSIMP_RAPIDJSON_NO_MEMBER_ITERATOR=OFF)
# endif()

# set(suffix "d")
# if (CMAKE_BUILD_TYPE  MATCHES Release)
# set(suffix "")
# endif()

# ExternalProject_Add(
#     ASSIMP
#     # GIT_REPOSITORY "https://github.com/assimp/assimp.git"
#     # GIT_TAG "v5.2.4"
#     SOURCE_DIR        ${CMAKE_CURRENT_LIST_DIR}/external/assimp
#     # BINARY_DIR        ${CMAKE_CURRENT_LIST_DIR}/external/assimp_build
#     BUILD_IN_SOURCE TRUE
#     GIT_SHALLOW 1 
#     UPDATE_COMMAND "" 
#     PATCH_COMMAND "" 
#     CMAKE_ARGS -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE} -DASSIMP_INSTALL=OFF -DASSIMP_BUILD_ASSIMP_VIEW=OFF -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -DJSONCPP_WITH_TESTS=OFF -DGLFW_BUILD_DOCS=OFF -DASSIMP_BUILD_ZLIB=ON -DASSIMP_BUILD_TESTS=OFF -DASSIMP_BUILD_ASSIMP_TOOLS=OFF -DASSIMP_RAPIDJSON_NO_MEMBER_ITERATOR=OFF
#     TEST_COMMAND ""
#     INSTALL_COMMAND ""
# ) 
# #https://cmake.org/cmake/help/latest/guide/importing-exporting/index.html
# add_library(assimp SHARED IMPORTED )
# set_property(TARGET assimp PROPERTY
#              IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/external/assimp/bin/assimp-vc143-mt${suffix}.dll)
# set_property(TARGET assimp PROPERTY
#              IMPORTED_IMPLIB ${CMAKE_CURRENT_LIST_DIR}/external/assimp/lib/assimp-vc143-mt${suffix}.lib)
# target_include_directories(assimp INTERFACE ${CMAKE_CURRENT_LIST_DIR}/external/assimp/include)
# add_library(assimp::assimp ALIAS assimp)

# file(COPY ${CMAKE_CURRENT_LIST_DIR}/external/assimp_build/bin/assimp-vc143-mt${suffix}.dll DESTINATION ${CMAKE_BINARY_DIR}/bin)

# set(ASSIMP_INCLUDE_DIR  ${CMAKE_CURRENT_LIST_DIR}/external/assimp_src/include  ${CMAKE_CURRENT_LIST_DIR}/external/assimp_build/include)
# set(ASSIMP_LIB_DIR  ${CMAKE_CURRENT_LIST_DIR}/external/assimp_build/lib/assimp-vc143-mt${suffix}.lib)

message("jsoncpp")
add_subdirectory(external/jsoncpp)
message("GLM")
add_subdirectory(external/glm/)

message("Python")
set(Python3_ROOT_DIR ${CMAKE_BINARY_DIR}/bin/python/)
find_package(Python3 3.10 EXACT COMPONENTS Interpreter Development REQUIRED)
GET_TARGET_PROPERTY(\_lib Python3::Python IMPORTED_IMPLIB)
MESSAGE(STATUS "Python3::Python ${\_lib}")
GET_TARGET_PROPERTY(\_lib Python3::Python IMPORTED_IMPLIB_DEBUG)
MESSAGE(STATUS "Python3::Python (Debug) ${\_lib}")
GET_TARGET_PROPERTY(\_lib Python3::Python IMPORTED_IMPLIB_RELEASE)
MESSAGE(STATUS "Python3::Python (Release) ${\_lib}")

message("pybind11")
add_subdirectory(external/pybind11)
message(STATUS "Found pybind11 v${pybind11_VERSION}: ${pybind11_INCLUDE_DIRS} ${pybind11_DEFINITIONS} ${pybind11_LIBRARIES}")
message("==========================================================")


add_subdirectory(src)
add_dependencies (imgui glfw)
add_dependencies(STB_IMAGE imgui)
add_dependencies(glcpp assimp)