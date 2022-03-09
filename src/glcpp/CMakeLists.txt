set(GLCPP_SRC_FILES
    main.cpp
    shader.h
    shader.cpp
    )
add_executable(glcpp  ${GLCPP_SRC_FILES})

# 컴파일 옵션 설정
target_compile_options(glcpp PRIVATE -Wall -Werror -O3 -std=c++17)

if(WIN32)
  target_include_directories(glcpp PUBLIC ./ ./../../external)
  target_link_directories(glcpp PUBLIC ../../external ../../external/lib)
  target_link_libraries(
    glcpp
    glfw
    glm
    -lpthread
    -lglad
    -lgdi32
    -lopengl32
    -static)
endif(WIN32)