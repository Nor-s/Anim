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
    glcpp/anim/assimp_animation.hpp
    glcpp/anim/json_animation.hpp
    glcpp/anim/animator.hpp
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
./UI/imgui_context.h 
./UI/imgui_scene_window.h
./UI/imgui_text_edit.h
./UI/imgui_json.h
./pixel3D.h 
./scene/scene.hpp
./scene/scene1.hpp
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



