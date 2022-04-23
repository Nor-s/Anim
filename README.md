# **n-glcpp**

opengl project

## TODO

-   애니메이션 재생/ 스탑 (5)

-   GUI 작업 (4)

    -   애니메이션 관련(본, 애니메이션 재생 등)

-   object 셀렉트 (1)

-   그리드 평면 (1)

## bug fix

-   몇몇 mixamo 모델에서 애니메이션 오류
    -   이유?
        -   바인딩포즈는 로딩되므로, 애니메이션 변환 행렬 문제일 가능성이 있음.
    -   해결
        -   $AssimpFbx$\_PreRotation, $AssimpFbx$\_PreTranslation
        -   https://github.com/assimp/assimp/issues/1974
        -   assimp에서 모델을 임포트할 때 생기는 문제였다. 아래의 코드를 삽입하면 해결된다.
        -   `import.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);`

## external

-   glad: https://glad.dav1d.de/

-   GLFW: https://www.glfw.org/

-   GLM: https://glm.g-truc.net/0.9.9/index.html

-   Assimp: https://github.com/assimp/assimp

-   ImGUI: https://github.com/ocornut/imgui

-   stb: https://github.com/nothings/stb

-   nfd(extended): https://github.com/btzy/nativefiledialog-extended

-   jsoncpp: https://github.com/open-source-parsers/jsoncpp

## screenshot

### 3/25

-   test model loading, pixelate, imgui

![](https://github.com/Nor-s/n-glcpp/blob/main/screenshot/Mar-25-2022%2012-58-15.gif?raw=true)

### 4/1

-   ImGui docking, refactoring, camera, skybox blur

![](<https://github.com/Nor-s/n-glcpp/blob/main/screenshot/Animation%20(32).gif?raw=true>)

-   3D model to png, pixelate

![](/screenshot/Apr-01-2022%2014-53-09.gif)

### 4/9

-   outline, outline color, animation

![](/screenshot/Apr-09-2022%2004-14-49.gif)

### 4/17

-   I studied how to move around with code. (I think this is an inefficient method)

    1. get `aiNode ::mTransformation`
    2. decompose this matrix
    3. update translation, rotation, scale

![](/screenshot/Apr_2022-04-17_12-45-38.png)

```cpp
            glm::mat4 nodeTransform = node->transformation;

            glm::vec3 scale;
            glm::quat rotation;
            glm::vec3 translation;
            glm::vec3 skew;
            glm::vec4 perspective;

            glm::decompose(nodeTransform, scale, rotation, translation, skew, perspective);

            if (nodeName == "RightUpLeg")
                rotation.x += 0.5;
            if (nodeName == "LeftUpLeg")
                rotation.x -= 0.5;
            if (nodeName == "RightArm")
                rotation.y += 0.5;
            if (nodeName == "LeftArm")
                rotation.y += 0.5;

            auto tt = glm::translate(glm::mat4(1.0f), translation);
            auto ss = glm::scale(glm::mat4(1.0f), scale);
            auto rr = glm::toMat4(rotation);

            nodeTransform = tt * rr * ss;
```

### 4/23

-   gui for hierarchy, gui for transformation
    ![](/screenshot/Animation39.gif)

## build

### windows10, 11

-   MSYS2를 설치하고 아래 패키지 다운

```
pacman -S mingw-w64-x86_64-gcc

pacman -S cmake

pacman -S mingw-w64-x86_64-cmake

pacman -S mingw-w64-x86_64-ninja

pacman -S mingw-w64-x86_64-glfw

pacman -S mingw-w64-x86_64-glm

pacman -S mingw-w64-x86_64-assimp
```

-   MSYS2 환경변수 설정
-   VSCODE에서 C++ 확장자 팩을 다운받고, CMAKE TOOL 세팅에서 GENERATOR 를 NINJA 선택.
-   `glt clone https://github.com/Nor-s/n-glcpp.git` 명령어로 레포를 받고
-   vscode 에서 n-glcpp 폴더를 열고, cmake 설정을하고 (gcc)
-   vscode 하단에 build 버튼 누르면 빌드된다.

## references

-   [render: learnopengl](https://learnopengl.com/)

-   [texture to file](https://stackoverflow.com/questions/11863416/read-texture-bytes-with-glreadpixels)
-   [pixelate shader](https://github.com/genekogan/Processing-Shader-Examples/blob/master/TextureShaders/data/pixelate.glsl)

-   [skeleton animation: ogldev](https://ogldev.org/www/tutorial38/tutorial38.html)

-   [shader1](https://lettier.github.io/3d-game-shaders-for-beginners/)
-   [shader2](https://thebookofshaders.com/)
