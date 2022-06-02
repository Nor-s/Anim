# **Pixel3D v0.0.4**

3D Model to Pixel Art with mediapipe

![](/screenshot/2022-05-01-1.05.03.png)


## external

-   glad: https://glad.dav1d.de/

-   GLFW: https://www.glfw.org/

-   GLM: https://glm.g-truc.net/0.9.9/index.html

-   Assimp: https://github.com/assimp/assimp

-   ImGUI: https://github.com/ocornut/imgui

    -   timeline: https://gitlab.com/GroGy/im-neo-sequencer
    -   text editor: https://github.com/BalazsJako/ImGuiColorTextEdit

-   stb: https://github.com/nothings/stb

-   nfd(extended): https://github.com/btzy/nativefiledialog-extended

-   jsoncpp: https://github.com/open-source-parsers/jsoncpp

## resources

-   mixamo: https://www.mixamo.com/#/?page=1&type=Character
-   d2coding font: https://github.com/naver/d2codingfont/wiki/Open-Font-License

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

-   custom animation

    1. get `aiNode ::mTransformation`
    2. decompose this matrix
    3. update translation, rotation, scale

-   or

    1. get `aiNode::mTransformation`
    2. mTransformation = mTransformation *T*R\*S
       ![](/screenshot/Apr_2022-04-17_12-45-38.png)

### 4/23

-   gui for hierarchy, gui for transformation

    ![](/screenshot/anim.gif)

### 4/29

-   animation gui: simple timeline
    ![](/screenshot/Apr-30-2022-00-11-45.gif)
-   retargeting(same bone hierarchy: mixamo): model B _ animation B^-1 _ (animation TRS)
    ![](/screenshot/Apr-30-2022-00-32-28.gif)

### 5/1

-   animation with json file, text edit

    ![](/screenshot/May-01-2022-10-54-32.gif)

### 5/28

-   Add mediapipe open button

    ![](/screenshot/animation22.gif)

### 6/02

- Add export button: glft2
- Below is an screenshot in Blender.
    ![](/screenshot/screenshot.gif)

## build

### windows10, 11

-   MSYS2를 설치하고 아래 패키지 다운

```
pacman -S mingw-w64-x86_64-gcc

pacman -S cmake

pacman -S mingw-w64-x86_64-cmake

pacman -S mingw-w64-x86_64-ninja
```

-   MSYS2 환경변수 설정
-   VSCODE에서 C++ 확장자 팩을 다운받고, CMAKE TOOL 세팅에서 GENERATOR 를 NINJA 선택.
-   `glt clone  --recursive https://github.com/Nor-s/n-glcpp.git` 명령어로 레포를 받고
-   vscode 에서 n-glcpp 폴더를 열고, cmake 설정 (gcc)
-   vscode 하단에 build 버튼 눌러서 빌드.

## references

-   [learnopengl](https://learnopengl.com/)

-   [texture to file](https://stackoverflow.com/questions/11863416/read-texture-bytes-with-glreadpixels)
-   [pixelate shader](https://github.com/genekogan/Processing-Shader-Examples/blob/master/TextureShaders/data/pixelate.glsl)

-   [skeleton animation: ogldev](https://ogldev.org/www/tutorial38/tutorial38.html)

-   [shader1](https://lettier.github.io/3d-game-shaders-for-beginners/)
-   [shader2](https://thebookofshaders.com/)
