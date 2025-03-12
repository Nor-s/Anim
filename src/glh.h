#ifndef ANIM_GL
#define ANIM_GL

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>

#else
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#endif

#endif