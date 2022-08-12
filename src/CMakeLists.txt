# opengl library wrapper
set(GLCPP_SRC
    glcpp/window.cpp
    glcpp/shader.cpp
    glcpp/mesh.cpp
    glcpp/model.cpp
    glcpp/camera.cpp
    glcpp/cubemap.cpp
    glcpp/framebuffer.cpp
    glcpp/application.hpp
    glcpp/importer.cpp
    glcpp/exporter.cpp
    glcpp/component/component.hpp
    glcpp/component/transform_component.cpp
    glcpp/component/animation_component.cpp
    glcpp/component/OBB_component.cpp
    glcpp/utility.hpp
    glcpp/anim/bone.hpp
    glcpp/anim/animation.hpp
    glcpp/anim/assimp_animation.hpp
    glcpp/anim/json_animation.hpp
    glcpp/animator.cpp
    glcpp/entity.cpp
    glcpp/image.cpp
    glcpp/armature.cpp
)

add_library(glcpp STATIC ${GLCPP_SRC})

set(LIBs STB_IMAGE imgui glad  jsoncpp_static)


if(MSVC)
   target_compile_options(glcpp PUBLIC  "/utf-8")
  if (CMAKE_BUILD_TYPE MATCHES Release)
    target_link_options(glcpp PUBLIC "/INCREMENTAL" "/SUBSYSTEM:windows" "/ENTRY:mainCRTStartup")
 endif()
  target_link_libraries(glcpp PUBLIC ${LIBs} glm assimp::assimp)
  target_include_directories(glcpp PUBLIC ./ ./glcpp ../ ${CMAKE_HOME_DIRECTORY}/external)


elseif(WIN32)
    target_compile_options(glcpp PRIVATE -Wall -Werror -std=c++17 -static)

    target_include_directories(glcpp PUBLIC ./ ./glcpp ../ ${CMAKE_HOME_DIRECTORY}/external)
    target_link_libraries(glcpp PUBLIC ${LIBs} assimp::assimp glm -lpthread -lgdi32 -lopengl32 -static)
else()
    target_compile_options(glcpp PRIVATE -Wall -Werror -std=c++17)

    target_include_directories(glcpp PUBLIC ./ ./glcpp ../ ${CMAKE_HOME_DIRECTORY}/external)
    target_link_libraries(glcpp PUBLIC ${LIBs} assimp glm::glm -ldl -lpthread)
endif()

message("==========================================================")
# python
add_executable(py cpython/main.cpp)
target_link_libraries(py PRIVATE Python3::Python pybind11::embed)


message("==========================================================")

add_executable(Anim 
./main.cpp 
./pixelate_framebuffer.cpp 
 
./UI/hierarchy_layer.cpp
./UI/model_property_layer.cpp
./UI/scene_layer.cpp
./UI/text_edit_layer.cpp
./UI/timeline_layer.cpp
./UI/main_layer.cpp
./UI/imgui_helper.cpp
./UI/ImFileDialog.cpp

./mp2mm.cpp
./app.cpp
./scene/scene.hpp
./scene/main_scene.cpp
./scene/shared_resources.cpp
)

if(MSVC)
  target_compile_options(Anim PUBLIC   "/utf-8")#  target_link_options(Anim PUBLIC "/INCREMENTAL")
  if (CMAKE_BUILD_TYPE  MATCHES Release)
      target_link_options(Anim PUBLIC "/INCREMENTAL" "/SUBSYSTEM:windows" "/ENTRY:mainCRTStartup")
  endif()
  target_link_libraries(Anim  glcpp)
elseif(WIN32)
    target_compile_options(Anim  PRIVATE -Wall -Werror -std=c++17 -static)
    target_link_libraries(Anim  glcpp)
    target_include_directories(Anim  PUBLIC ./)
else()
    target_link_libraries(Anim  glcpp)
endif()

message("==========================================================")