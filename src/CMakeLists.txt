# opengl library wrapper
set(GLCPP_SRC
    glcpp/window.cpp
    glcpp/window.h
    glcpp/shader.h
    glcpp/shader.cpp
    glcpp/mesh.cpp
    glcpp/mesh.h
    glcpp/model.cpp
    glcpp/model.h
    glcpp/camera.cpp
    glcpp/camera.h
    glcpp/cubemap.cpp
    glcpp/cubemap.h
    glcpp/framebuffer.cpp
    glcpp/framebuffer.h
    glcpp/application.hpp
    glcpp/transform_component.h
    glcpp/utility.hpp
    glcpp/anim/bone.hpp
    glcpp/anim/animation.hpp
    glcpp/anim/animator.hpp
)

add_library(glcpp STATIC ${GLCPP_SRC})
target_compile_options(glcpp PRIVATE -Wall -Werror -std=c++17)

set(LIBs STB_IMAGE assimp imgui glad nfd)

if(WIN32)
    target_include_directories(glcpp PUBLIC ./ ./glcpp ../ ${CMAKE_HOME_DIRECTORY}/external)
    target_link_libraries(glcpp PUBLIC ${LIBs} glm -lpthread -lgdi32 -lopengl32)
endif(WIN32)

if(APPLE) 
    target_include_directories(glcpp PUBLIC ./ ../ ${CMAKE_HOME_DIRECTORY}/external)
    target_link_libraries(glcpp PUBLIC ${LIBs} glm::glm -ldl -lpthread)
endif(APPLE)


add_executable(test ./test/test.cpp)

target_compile_options(test PRIVATE -Wall -Werror -std=c++17)
target_include_directories(test PUBLIC ./)
target_link_libraries(test glcpp)


add_executable(pixel3D 
./main.cpp 
./pixelate_framebuffer.cpp 
./UI/imgui_context.h 
./UI/imgui_scene_window.h
./pixel3D.cpp 
./pixel3D.h 
./scene/scene.hpp
./scene/scene1.hpp
./scene/bone_scene.hpp
)

target_compile_options(pixel3D  PRIVATE -Wall -Werror -std=c++17)
target_include_directories(pixel3D  PUBLIC ./)
target_link_libraries(pixel3D  glcpp jsoncpp_static)


