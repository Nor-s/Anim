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
  VERSION 0.0.5)
set(CMAKE_CXX_STANDARD 17)
if (UNIX AND NOT APPLE)
    set(CMAKE_MAKE_PROGRAM /usr/bin/ninja CACHE FILEPATH "")
endif()
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

file(COPY resources DESTINATION ${CMAKE_BINARY_DIR}/bin)
if (APPLE)
  execute_process(COMMAND sh "install_mp2mm.sh"
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
else()
  execute_process(COMMAND cmd /C install_mp2mm.bat
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
endif()
file(COPY ${CMAKE_CURRENT_LIST_DIR}/dist/mp2mm DESTINATION ${CMAKE_BINARY_DIR}/bin)

message(STATUS "Compiler")
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
#option(ASSIMP_INSTALL ON)
# option(ASSIMP_BUILD_ASSIMP_VIEW OFF)
# option(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT ON)

message("GLAD")
add_subdirectory(external/glad/)
message("STB")
add_library(STB_IMAGE "external/stb/stb_image.cpp")


if(WIN32)
  message("GLFW")
  add_subdirectory(external/glfw/)
  message("IMGUI")
  add_subdirectory(external/imgui/)
  if(MSVC) 
    message("ASSIMP")
    add_subdirectory(external/assimp/)
  else()          
    # find_package(assimp CONFIG REQUIRED)
    message("ASSIMP")
    add_subdirectory(external/assimp/)
  endif()
else()
  add_subdirectory(external/imgui/)
  add_subdirectory(external/assimp/)
  add_subdirectory(external/glfw/)
endif()

message("NFD")
add_subdirectory(external/nfd)
message("jsoncpp")
add_subdirectory(external/jsoncpp)
message("GLM")
add_subdirectory(external/glm/)


add_subdirectory(src)
add_dependencies (imgui glfw)
add_dependencies(STB_IMAGE imgui)

