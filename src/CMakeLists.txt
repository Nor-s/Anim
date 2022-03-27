find_package(glfw3 REQUIRED FATAL_ERROR)

find_package(glm REQUIRED FATAL_ERROR)

add_subdirectory(glcpp)


add_executable(render ./main.cpp ./class/3d_to_pixel.h)

target_compile_options(render PRIVATE -Wall -Werror -std=c++17)
target_include_directories(render PUBLIC ./)
target_link_libraries(render glcpp nfd)