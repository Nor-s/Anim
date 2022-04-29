#ifndef SRC_SCENE_SCENE_H
#define SRC_SCENE_SCENE_H

#include "glcpp/transform_component.h"
#include "glcpp/model.h"
#include "glcpp/framebuffer.h"
#include "glcpp/camera.h"
#include "glcpp/anim/animation.hpp"
#include "UI/imgui_option.h"
#include <memory>

class Scene
{
public:
    virtual std::shared_ptr<glcpp::Model> &get_model() = 0;
    virtual std::shared_ptr<glcpp::Framebuffer> &get_framebuffer() = 0;
    virtual std::shared_ptr<glcpp::Camera> &get_camera() = 0;
    virtual void init_framebuffer(uint32_t width, uint32_t height) = 0;
    virtual void pre_draw() = 0;
    virtual void draw() = 0;
    virtual void add_model(const char *file_name) = 0;
    virtual void set_size(uint32_t width, uint32_t height) = 0;
    virtual void print_to_png(const std::string &file_name) = 0;
    virtual ui::ImguiOption &get_option()
    {
        return imgui_option_;
    }
    virtual glcpp::Animator *get_mutable_animator()
    {
        return nullptr;
    }

protected:
    ui::ImguiOption imgui_option_;
};
#endif