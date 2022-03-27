#ifndef RENDER_3D_TO_PIXEL_H
#define RENDER_3D_TO_PIXEL_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glcpp/shader.h>
#include <glcpp/camera.h>
#include <glcpp/model.h>
#include <glcpp/window.h>
#include <glcpp/cubemap.h>
#include <glcpp/framebuffer.h>

#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_glfw.h>

#include <iostream>
#include <vector>
#include <nfd.h>
#include <memory>

class OneModelToPixel
{
    // namespace fs = std::filesystem;
    // int pixelate_factor = 1000;
    // std::unique_ptr<glcpp::Framebuffer> model_framebuffer;
    // glcpp::Model ourModel(fs::canonical(fs::path("./../../resources/models/nanosuit/nanosuit.obj")).string().c_str());
    // void draw_model(glcpp::Model &ourModel, Shader &ourShader, const glm::mat4 &view, const glm::mat4 &projection);
    // glEnable(GL_DEPTH_TEST);
    // glEnable(GL_BLEND);
    // glBindFramebuffer(GL_FRAMEBUFFER, skybox_framebuffer->get_fbo());
    // {
    //     // skybox capture
    //     glViewport(0, 0, skybox_framebuffer->get_width(), skybox_framebuffer->get_height());
    //     glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //     skybox.draw(camera.GetViewMatrix(), projection);
    // }
    // glBindFramebuffer(GL_FRAMEBUFFER, model_framebuffer->get_fbo());
    // {
    //     glEnable(GL_DEPTH_TEST);
    //     glViewport(0, 0, model_framebuffer->get_width(), model_framebuffer->get_height());
    //     glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //     //glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    //     //glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
    //     draw_model(ourModel, ourShader, view, projection);
    //     skybox.draw(camera.GetViewMatrix(), projection);
    // }
    // glBindFramebuffer(GL_FRAMEBUFFER, framebuffer1.get_fbo());
    // {
    //     glEnable(GL_DEPTH_TEST);
    //     glEnable(GL_BLEND);
    //     glEnable(GL_STENCIL);
    //     glViewport(0, 0, framebuffer1.get_width(), framebuffer1.get_height());
    //     glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    //     //glDisable(GL_BLEND);
    //     // glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    //     // glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
    //     //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    //     // glColorMask(false, false, false, true);
    //     //glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
    //     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //     model_framebuffer->draw();
    //     model_framebuffer->print_color_texture("first.png");
    //     framebuffer1.print_color_texture("file.png");
    // }
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // {
    //     glViewport(0, 0, g_window.get_width(), g_window.get_height());
    //     glEnable(GL_DEPTH_TEST);
    //     glEnable(GL_BLEND);

    //     glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //     glDepthMask(GL_FALSE);
    //     if (is_skybox_blur)
    //     {
    //         skybox_framebuffer->draw();
    //     }
    //     else
    //     {
    //         skybox.draw(camera.GetViewMatrix(), projection);
    //     }
    //     glDepthMask(GL_TRUE);
    //     //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //     //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    //     //   model_framebuffer->set_pixelate_factor(pixelate_factor);
    //     model_framebuffer->draw();
    // }
};

#endif