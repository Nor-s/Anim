# **Pixel3D v0.0.4**


![](/screenshot/2022-05-01-1.05.03.png)

## build

### windows10, 11

-   install MSYS2, git, vscode

```
pacman -S mingw-w64-x86_64-gcc

pacman -S cmake

pacman -S mingw-w64-x86_64-cmake

pacman -S mingw-w64-x86_64-ninja

pacman -S mingw-w64-x86_64-assimp
```

-   MSYS2 Environment variable setting
-   VSCODE: C++ extension pack, CMAKE TOOL setting>GENERATOR>Ninja
-   `glt clone  --recursive https://github.com/Nor-s/Pixel3D.git`
## resources

-   mixamo: https://www.mixamo.com/#/?page=1&type=Character
-   d2coding font: https://github.com/naver/d2codingfont/wiki/Open-Font-License

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
## references

-   [learnopengl](https://learnopengl.com/)

-   [texture to file](https://stackoverflow.com/questions/11863416/read-texture-bytes-with-glreadpixels)
-   [pixelate shader](https://github.com/genekogan/Processing-Shader-Examples/blob/master/TextureShaders/data/pixelate.glsl)

-   [skeleton animation: ogldev](https://ogldev.org/www/tutorial38/tutorial38.html)

-   [shader1](https://lettier.github.io/3d-game-shaders-for-beginners/)
-   [shader2](https://thebookofshaders.com/)
