cmake_minimum_required (VERSION 3.20)

project(imguiTest)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -Wno-missing-braces")


add_executable(index
    ./main.cpp
)


set(EXTERNAL_LIBS STB_IMAGE imgui glad jsoncpp_static assimp::assimp glm)

target_compile_options(index PUBLIC "--use-port=contrib.glfw1")
target_link_options(index PUBLIC "--use-port=contrib.glfw1")

target_link_libraries(index PUBLIC ${EXTERNAL_LIBS})   
target_include_directories(index PUBLIC ./ ../ ${CMAKE_HOME_DIRECTORY}/external)

set_target_properties(index PROPERTIES LINK_FLAGS
    "-s WASM=-1 -s USE_GLFW=3 -s USE_WEBGL2=1"
)