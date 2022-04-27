cmake_minimum_required(VERSION 3.0)
project(Imgui)

# find_package(glfw3 REQUIRED FATAL_ERROR)
# Set where the ImGui files are stored
set(IMGUI_PATH ${CMAKE_CURRENT_SOURCE_DIR})
    
# Compile as static library 
file(GLOB IMGUI_SOURCES ${IMGUI_PATH}/*.cpp) 
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC ${IMGUI_PATH})
target_link_libraries(imgui PUBLIC glfw)