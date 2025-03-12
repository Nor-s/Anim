


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -Wno-missing-braces")

set(SRC_DIR ${CMAKE_HOME_DIRECTORY}/src)
set(GLCPP_SRC
    ${SRC_DIR}/glcpp/window.cpp
    ${SRC_DIR}/glcpp/camera.cpp
    ${SRC_DIR}/glcpp/application.hpp
)
if(APPLE)
  SET(CMAKE_OSX_DEPLOYMENT_TARGET 12.4)
endif()

add_library(glcpp STATIC ${GLCPP_SRC})

set(LIBs STB_IMAGE imgui jsoncpp_static)

set_target_properties(glcpp PROPERTIES LINK_FLAGS
    "-s WASM=-1 -s USE_GLFW=3 -s USE_WEBGL2=1"
)

target_compile_options(glcpp PUBLIC "--use-port=contrib.glfw3")
target_link_options(glcpp PUBLIC "--use-port=contrib.glfw3")

target_compile_options(glcpp PRIVATE -Wall -Werror -std=c++20)

target_include_directories(glcpp PUBLIC ${SRC_DIR}/ ${SRC_DIR}/glcpp ${CMAKE_HOME_DIRECTORY}/.. ${CMAKE_HOME_DIRECTORY}/external)
target_link_libraries(glcpp PUBLIC ${LIBs} assimp glm::glm -ldl -lpthread)

message("==========================================================")

# new
set(test_src
    ${SRC_DIR}/graphics/mesh.cpp
    ${SRC_DIR}/graphics/opengl/gl_shader.cpp
    ${SRC_DIR}/graphics/opengl/gl_mesh.cpp
    ${SRC_DIR}/graphics/opengl/framebuffer.cpp
    ${SRC_DIR}/graphics/opengl/image.cpp
    ${SRC_DIR}/graphics/post_processing.cpp
    ${SRC_DIR}/graphics/shader.h
    ${SRC_DIR}/graphics/mesh.h
    ${SRC_DIR}/graphics/opengl/gl_shader.h
    ${SRC_DIR}/graphics/opengl/gl_mesh.h
    ${SRC_DIR}/graphics/opengl/framebuffer.h
    ${SRC_DIR}/graphics/opengl/image.h
    ${SRC_DIR}/graphics/post_processing.h
    ${SRC_DIR}/util/log.h
    ${SRC_DIR}/util/utility.h
    ${SRC_DIR}/entity/entity.cpp
    ${SRC_DIR}/entity/components/ik_control_component.cpp
    ${SRC_DIR}/entity/components/realtime_component.cpp
    ${SRC_DIR}/entity/components/transform_component.cpp
    ${SRC_DIR}/entity/components/animation_component.cpp
    ${SRC_DIR}/entity/components/pose_component.cpp
    ${SRC_DIR}/entity/components/morphtarget_component.cpp
    ${SRC_DIR}/entity/components/renderable/armature_component.cpp
    ${SRC_DIR}/entity/components/renderable/mesh_component.cpp
    ${SRC_DIR}/entity/components/renderable/polygon_component.cpp
    ${SRC_DIR}/entity/entity.h
    ${SRC_DIR}/entity/components/component.h
    ${SRC_DIR}/entity/components/ik_control_component.h
    ${SRC_DIR}/entity/components/realtime_component.h
    ${SRC_DIR}/entity/components/transform_component.h
    ${SRC_DIR}/entity/components/animation_component.h
    ${SRC_DIR}/entity/components/pose_component.h
    ${SRC_DIR}/entity/components/renderable/mesh_component.h
    ${SRC_DIR}/entity/components/renderable/armature_component.h
    ${SRC_DIR}/entity/components/renderable/polygon_component.h
    ${SRC_DIR}/resources/exporter.cpp
    ${SRC_DIR}/resources/exporter.h
    ${SRC_DIR}/resources/importer.cpp
    ${SRC_DIR}/resources/importer.h
    ${SRC_DIR}/resources/model.cpp
    ${SRC_DIR}/resources/model.h
    ${SRC_DIR}/resources/shared_resources.cpp
    ${SRC_DIR}/resources/shared_resources.h
    ${SRC_DIR}/animation/animator.cpp
    ${SRC_DIR}/animation/animation.cpp
    ${SRC_DIR}/animation/json_animation.cpp
    ${SRC_DIR}/animation/assimp_animation.cpp
    ${SRC_DIR}/animation/bone.cpp
    ${SRC_DIR}/animation/retargeter.cpp
    ${SRC_DIR}/animation/morph_target.cpp
    ${SRC_DIR}/animation/animator.h
    ${SRC_DIR}/animation/animation.h
    ${SRC_DIR}/animation/assimp_animation.h
    ${SRC_DIR}/animation/json_animation.h
    ${SRC_DIR}/animation/bone.h
    ${SRC_DIR}/animation/retargeter.h
    ${SRC_DIR}/animation/morph_target.h
)
add_library(core STATIC ${test_src})


target_link_libraries(core PUBLIC ${LIBs} glm assimp::assimp)
target_include_directories(core PUBLIC
  ${SRC_DIR}
  ${SRC_DIR}/..
  ${SRC_DIR}/entity/
  ${SRC_DIR}/entity/components/
  ${SRC_DIR}/entity/components/renderable 
  ${SRC_DIR}/graphics 
  ${SRC_DIR}/graphics/opengl 
  ${SRC_DIR}/util
  ${SRC_DIR}/animation
  ${SRC_DIR}/resources
  ${CMAKE_HOME_DIRECTORY}/.. 
  ${CMAKE_HOME_DIRECTORY}/external)


message("==========================================================")

add_executable(Anim_wasm
./main.cpp 
 
${SRC_DIR}/UI/hierarchy_layer.cpp
${SRC_DIR}/UI/component_layer.cpp
${SRC_DIR}/UI/scene_layer.cpp
${SRC_DIR}/UI/scene_layer.h
${SRC_DIR}/UI/text_edit_layer.cpp
${SRC_DIR}/UI/timeline_layer.cpp
${SRC_DIR}/UI/main_layer.cpp
${SRC_DIR}/UI/main_layer.h
${SRC_DIR}/UI/imgui_helper.cpp
${SRC_DIR}/UI/ui_context.h

${SRC_DIR}/event/event_history.h
${SRC_DIR}/event/event_history.cpp

${SRC_DIR}/event/edit_function.cpp
${SRC_DIR}/event/edit_function.h

${SRC_DIR}/app.cpp
${SRC_DIR}/scene/scene.hpp
${SRC_DIR}/scene/main_scene.cpp
)

target_link_libraries(Anim_wasm  glcpp core)
target_include_directories(Anim_wasm  PUBLIC ./ ${SRC_DIR}/..)


target_link_libraries(Anim_wasm PUBLIC ${EXTERNAL_LIBS})   
target_include_directories(Anim_wasm PUBLIC ./ ${CMAKE_HOME_DIRECTORY}/src/.. ${CMAKE_HOME_DIRECTORY}/.. ${CMAKE_HOME_DIRECTORY}/external)

set_target_properties(Anim_wasm PROPERTIES LINK_FLAGS
    "-s WASM=-1 -s USE_GLFW=3 -s USE_WEBGL2=1 -s FORCE_FILESYSTEM=1 --preload-file resources -s ALLOW_MEMORY_GROWTH"
)

# target_compile_options(Anim PUBLIC "--use-port=contrib.glfw1")
# target_link_options(Anim PUBLIC "--use-port=contrib.glfw1")
message("==========================================================")