#include "main_scene.h"
#include <glcpp/camera.h>
#include <entity/entity.h>
#include <graphics/opengl/image.h>
#include <graphics/opengl/framebuffer.h>
#include <graphics/shader.h>

#include <util/log.h>
#include <graphics/shader.h>
#include <graphics/opengl/image.h>
#include <graphics/opengl/framebuffer.h>

#include <filesystem>

namespace fs = std::filesystem;

MainScene::MainScene(uint32_t width, uint32_t height, std::shared_ptr<anim::SharedResources> resources)
{
    anim::LOG("MainScene()");
    if (resources == nullptr)
    {
        resources_ = std::make_shared<anim::SharedResources>();
    }
    else
    {
        resources_ = std::move(resources);
    }
    init_framebuffer(width, height);
    grid_framebuffer_.reset(new anim::Image{100, 100, GL_RGBA});

    init_camera();
}

void MainScene::init_framebuffer(uint32_t width, uint32_t height)
{
    set_size(width, height);

    framebuffer_.reset(new anim::Framebuffer{width, height, GL_RGB, true});
    if (framebuffer_->error())
    {
        anim::LOG("framebuffer error");
        framebuffer_.reset(new anim::Framebuffer{width, height, GL_RGB, false});
    }
}

void MainScene::init_camera()
{
    camera_ = std::make_shared<glcpp::Camera>(glm::vec3(0.0f, 100.0f, 500.0f));
}

void MainScene::pre_draw()
{
    update_framebuffer();
    camera_->set_view_and_projection(framebuffer_->get_aspect());
    resources_->set_ubo_view(camera_->get_view());
    resources_->set_ubo_projection(camera_->get_projection());

    draw_to_framebuffer();
}

void MainScene::update_framebuffer()
{
    if (framebuffer_->get_width() != width_ || framebuffer_->get_height() != height_)
    {
        init_framebuffer(width_, height_);
    }
}

void MainScene::draw_to_framebuffer()
{
    auto grid_shader = resources_->get_mutable_shader("grid");
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    framebuffer_->bind_with_depth(background_color_);
    {
        resources_->update();

        grid_framebuffer_->draw(*grid_shader);
    }
    framebuffer_->unbind();
#ifndef NDEBUG
    auto error = glGetError();
    anim::LOG("main scene: " + std::to_string((int)error), error == GL_NO_ERROR);
#endif
}

void MainScene::draw()
{
    auto framebuffer_shader = resources_->get_mutable_shader("framebuffer");

    framebuffer_->draw(*framebuffer_shader);
}
