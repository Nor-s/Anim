set(GLCPP_SRC_FILES
    shader.h
    shader.cpp
    mesh.h
    model.h
    camera.h
    window.cpp
    window.h
    cubemap.cpp
    cubemap.h
    framebuffer.h
    )
add_library(glcpp  ${GLCPP_SRC_FILES})

# 컴파일 옵션 설정
target_compile_options(glcpp PRIVATE -Wall -Werror -std=c++17)

if(WIN32)
  target_include_directories(glcpp PUBLIC ./ ../ ./../../external)
  target_link_libraries(
    glcpp
  STB_IMAGE
  assimp
  imgui
  glad
    glm
    -lpthread
    -lgdi32
    -lopengl32
    )
endif(WIN32)

if(APPLE) 
target_include_directories(glcpp PUBLIC ./ ../ ./../../external)
target_link_libraries(glcpp PUBLIC
  STB_IMAGE
  assimp
  imgui
  glm::glm
  glad
  -ldl
  -lpthread)
endif(APPLE)