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
    glcpp/utility.hpp
    glcpp/anim/bone.hpp
    glcpp/anim/animation.hpp
    glcpp/anim/assimp_animation.hpp
    glcpp/anim/json_animation.hpp
    glcpp/animator.cpp
    glcpp/entity.cpp
)

add_library(glcpp STATIC ${GLCPP_SRC})

set(LIBs STB_IMAGE imgui glad nfd jsoncpp_static)


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



add_executable(pixel3D 
./main.cpp 
./pixelate_framebuffer.cpp 
 
./UI/hierarchy_layer.cpp
./UI/model_property_layer.cpp
./UI/scene_layer.cpp
./UI/text_edit_layer.cpp
./UI/timeline_layer.cpp
./UI/main_layer.cpp
./UI/imgui_helper.hpp


./mp2mm.cpp
./pixel3D.cpp
./scene/scene.hpp
./scene/main_scene.cpp
./scene/shared_resources.cpp
)
if(MSVC)
  message("------------------MSVC")
  target_compile_options(pixel3D PUBLIC   "/utf-8")#  target_link_options(pixel3D PUBLIC "/INCREMENTAL")
  if (CMAKE_BUILD_TYPE  MATCHES Release)
     message("--------------release")
      target_link_options(pixel3D PUBLIC "/INCREMENTAL" "/SUBSYSTEM:windows" "/ENTRY:mainCRTStartup")
  endif()
  target_link_libraries(pixel3D  glcpp)
elseif(WIN32)
    message("------------------GCC or...")
    target_compile_options(pixel3D  PRIVATE -Wall -Werror -std=c++17 -static)
    target_link_libraries(pixel3D  glcpp)
    target_include_directories(pixel3D  PUBLIC ./)
else()
   message("------------------APPLE")
    target_link_libraries(pixel3D  glcpp)
endif()



