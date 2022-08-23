cmake_minimum_required(VERSION 3.0)
project(Imgui)

# find_package(glfw3 REQUIRED FATAL_ERROR)
# Set where the ImGui files are stored
set(IMGUI_PATH ${CMAKE_CURRENT_SOURCE_DIR})
    
# Compile as static library 
file(GLOB IMGUI_SOURCES ${IMGUI_PATH}/*.cpp) 
set(IMGUI_ICONS
        icons/icons.cpp
        icons/folder.cpp
        icons/fontaudio.cpp
        icons/fa_brands.cpp
        icons/fa_regular.cpp
        icons/fa_solid.cpp
        icons/webfont.cpp
        icons/fork_webfont.cpp
        icons/material_icons.cpp
        icons/kenney.cpp
        icons/StepMath.cpp
    )
set(IMGUI_INC_DIRS
    ${IMGUI_PATH}
    ${IMGUI_PATH}/ImGuiFileDialog
    ${IMGUI_PATH}/icons
)
add_library(imgui STATIC ${IMGUI_SOURCES} ${IMGUI_ICONS} ImGuiFileDialog/ImGuiFileDialog.cpp)
target_include_directories(imgui PUBLIC ${IMGUI_INC_DIRS})
target_link_libraries(imgui PUBLIC glfw)