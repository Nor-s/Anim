# **Anim v0.0.1**

Animating tool

![](</screenshot/2022-08-21(1).gif>)

## **Feature**

-   Eidit: FK animation
-   Object selction
-   Embeded python (only windows)
-   Export glft2

## **TODO**

-   python script
-   compute shader for animation
-   Fix camera movement
-   Ctrl + z, Ctrl + c, Ctrl + v
-   IK edit mode
-   retargeting

## **Build**

-   windows: MSVC 19.33.31517.0
-   ~~ubunt(wsl2): gcc 9.4.0 x86_64-linux-gnu~~
-   ~~mac os: Clang 13.0.0 arm64-apple-darwin21.2.0~~

```
 mkdir build
 cd build
 cmake .. -G Ninja
 Ninja
```

## **External**

-   glad: https://glad.dav1d.de/

-   GLFW: https://www.glfw.org/

-   GLM: https://glm.g-truc.net/0.9.9/index.html

-   Assimp: https://github.com/assimp/assimp

-   ImGUI: https://github.com/ocornut/imgui

    -   timeline: https://gitlab.com/GroGy/im-neo-sequencer
    -   text editor: https://github.com/BalazsJako/ImGuiColorTextEdit
    -   dialog: https://github.com/dfranx/ImFileDialog
    -   ImGuizmo: https://github.com/CedricGuillemet/ImGuizmo

-   stb: https://github.com/nothings/stb

-   jsoncpp: https://github.com/open-source-parsers/jsoncpp

-   font(icon)

    -   https://github.com/juliettef/IconFontCppHeaders
    -   https://github.com/tanluteam/imgui

-   cpython

-   pybind11

## **Reference**

-   [learnopengl](https://learnopengl.com/)

-   [texture to file](https://stackoverflow.com/questions/11863416/read-texture-bytes-with-glreadpixels)

-   [pixelate shader](https://github.com/genekogan/Processing-Shader-Examples/blob/master/TextureShaders/data/pixelate.glsl)

-   [skeleton animation: ogldev](https://ogldev.org/www/tutorial38/tutorial38.html)

-   [blur shader](https://www.shadertoy.com/view/Xltfzj)

-   grid shader

    -   [grid shader 1](http://asliceofrendering.com/scene%20helper/2020/01/05/InfiniteGrid/)

    -   [grid shader 2](https://github.com/martin-pr/possumwood/wiki/Infinite-ground-plane-using-GLSL-shaders)

    -   [grid shader 3](https://github.com/blender/blender/blob/594f47ecd2d5367ca936cf6fc6ec8168c2b360d0/source/blender/draw/engines/overlay/shaders/grid_frag.glsl)

### **Resources**

-   model
    -   https://www.mixamo.com/#/?page=1&type=Character
    -   https://github.com/JoeyDeVries/LearnOpenGL/tree/master/resources/objects
-   font
    -   https://github.com/naver/d2codingfont/wiki/Open-Font-License
    -   https://github.com/juliettef/IconFontCppHeaders
-   skybox
    -   https://www.cleanpng.com/free/skybox.html
    -   https://opengameart.org/content/forest-skyboxes
    -   https://www.humus.name/index.php?page=Textures
