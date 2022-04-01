#ifndef SRC_PIXEL3D_H
#define SRC_PIXEL3D_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <vector>
#include "UI/imgui_context.h"
#include "scene/scene1.hpp"
#include "scene/scene2.hpp"

#include <filesystem>
#include "glcpp/application.hpp"
#include <stb/stb_image.h>
#include "glcpp/camera.h"
namespace fs = std::filesystem;

class Pixel3D : public glcpp::Application<Pixel3D>
{
    template <typename T>
    friend class glcpp::Application;

private:
    Pixel3D()
    {
    }

public:
    virtual ~Pixel3D()
    {
        imgui_.release();
        window_.release();
    }
    virtual void init(int width, int height, const std::string &title) override
    {
        stbi_set_flip_vertically_on_load(true);
        init_window(width, height, title);
        init_ui();
        scene_.reset(new Scene1(width, height));
        scene2_.reset(new Scene2(width, height));
    }
    void init_window(int width, int height, const std::string &title)
    {
        window_ = std::make_unique<glcpp::Window>(width, height, title);
        window_->set_factor();
        window_->set_user_pointer(this);
        init_callback();
    }
    void init_callback()
    {
        window_->set_framebuffer_size_callback(framebuffer_size_callback);
        window_->set_scroll_callback(scroll_callback);
        window_->set_mouse_button_callback(mouse_btn_callback);
        window_->set_cursor_pos_callback(mouse_callback);
    }
    void init_ui()
    {
        imgui_ = std::make_unique<ui::ImGuiContext>();
        imgui_->init(window_->get_handle());
    }
    virtual void loop() override
    {
        while (!window_->should_close())
        {
            float currentFrame = static_cast<float>(glfwGetTime());
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            processInput(window_->get_handle());
            pre_draw();
            {
                imgui_->begin();
                imgui_->dock_draw();

                scene_->pre_draw();
                scene2_->pre_draw();
                scene_->draw();
                imgui_->property_draw(scene_.get());

                imgui_->scene_draw("scene2", scene2_.get());
                imgui_->scene_draw("scene", scene_.get());
                imgui_->end();
            }
            post_draw();
        }
    }
    void pre_draw()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, window_->get_width(), window_->get_height());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    }
    void post_draw()
    {
        glfwSwapBuffers(window_->get_handle());
        glfwPollEvents();
    }

    void processInput(GLFWwindow *window)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            scene_->get_camera()->ProcessKeyboard(glcpp::FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            scene_->get_camera()->ProcessKeyboard(glcpp::BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            scene_->get_camera()->ProcessKeyboard(glcpp::LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            scene_->get_camera()->ProcessKeyboard(glcpp::RIGHT, deltaTime);
    }

    static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
    {
        auto app = reinterpret_cast<Pixel3D *>(glfwGetWindowUserPointer(window));

        app->window_->update_window();
    }

    static void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
    {
        auto app = reinterpret_cast<Pixel3D *>(glfwGetWindowUserPointer(window));
        if (app->is_pressed && app->scene_)
        {
            app->scene_->get_camera()->ProcessMouseMovement((static_cast<float>(yposIn) - app->prev_mouse.y) / 3.6f, (static_cast<float>(xposIn) - app->prev_mouse.x) / 3.6f);
            app->prev_mouse.x = xposIn;
            app->prev_mouse.y = yposIn;
        }
        if (app->is_pressed_scroll && app->scene_)
        {
            app->scene_->get_camera()->ProcessMouseScrollPress((static_cast<float>(yposIn) - app->prev_mouse.y), (static_cast<float>(xposIn) - app->prev_mouse.x), app->deltaTime);
            app->prev_mouse.x = xposIn;
            app->prev_mouse.y = yposIn;
        }
        app->cur_mouse.x = xposIn;
        app->cur_mouse.y = yposIn;
    }
    static void mouse_btn_callback(GLFWwindow *window, int button, int action, int mods)
    {
        auto app = reinterpret_cast<Pixel3D *>(glfwGetWindowUserPointer(window));
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && app->imgui_ && app->imgui_->is_window_hovered("scene"))
        {
            app->prev_mouse.x = app->cur_mouse.x;
            app->prev_mouse.y = app->cur_mouse.y;
            app->is_pressed = true;
        }
        else
        {
            app->is_pressed = false;
        }
        if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS && app->imgui_ && app->imgui_->is_window_hovered("scene"))
        {
            app->prev_mouse.x = app->cur_mouse.x;
            app->prev_mouse.y = app->cur_mouse.y;
            app->is_pressed_scroll = true;
        }
        else
        {
            app->is_pressed_scroll = false;
        }
    }

    static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
    {
        auto app = reinterpret_cast<Pixel3D *>(glfwGetWindowUserPointer(window));
        if (app->scene_ && app->imgui_ && app->imgui_->is_window_hovered("scene"))
            app->scene_->get_camera()->ProcessMouseScroll(yoffset);
    }

private:
    // timing
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    // mouse event
    bool is_pressed = false;
    bool is_pressed_scroll = false;
    glm::vec2 prev_mouse{-1.0f, -1.0f}, cur_mouse{-1.0f, -1.0f};
    std::unique_ptr<ui::ImGuiContext> imgui_;
    std::unique_ptr<Scene1> scene_;
    std::unique_ptr<Scene2> scene2_;
};

#endif
