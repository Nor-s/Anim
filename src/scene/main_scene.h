#ifndef SRC_SCENE_MAIN_SCENE_H
#define SRC_SCENE_MAIN_SCENE_H

#include <filesystem>
#include "scene.hpp"
namespace fs = std::filesystem;

class MainScene : public Scene
{
    virtual void init_framebuffer(uint32_t width, uint32_t height) = 0;
    virtual void pre_draw() = 0;
    virtual void draw() = 0;
    virtual void set_size(uint32_t width, uint32_t height) = 0;
    virtual void set_delta_time(float dt) = 0;
    virtual std::shared_ptr<glcpp::Framebuffer> get_framebuffer() = 0;
};

#endif
