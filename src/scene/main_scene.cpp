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

#include "../components/component.h"
#include "../components/pose_component.h"

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
    offscreen_framebuffer_.reset(new anim::Framebuffer{width, height, GL_RGB, false});
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
    // glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    framebuffer_->bind_with_depth_and_stencil(background_color_);
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

void MainScene::picking(int x, int y, bool is_only_bone)
{
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    offscreen_framebuffer_->bind_with_depth({1.0f, 1.0f, 1.0f, 1.0f});
    {
        resources_->update_for_picking();
    }
    offscreen_framebuffer_->unbind();
    auto pixel = offscreen_framebuffer_->read_pixel(x, y);
    anim::LOG(std::to_string(pixel.x) + " " + std::to_string(pixel.y) + " " + std::to_string(pixel.z));
    int pick_id = pixel.x + pixel.y * 256;
    if (pick_id == 0x0000ffff)
    {
        anim::LOG("- - pick: background");
        set_selected_entity(nullptr);
    }
    else
    {
        auto entity = resources_->get_entity(pick_id);
        if (entity)
        {
            set_selected_entity(entity->get_mutable_root());
            if (is_only_bone)
            {
                if (pixel.z == 255)
                {
                    set_selected_entity(entity);
                }
                else if (selected_entity_ && selected_entity_->get_component<anim::PoseComponent>())
                {
                    // mesh to armature
                    auto pose = selected_entity_->get_component<anim::PoseComponent>();

                    set_selected_entity(pose->find(pixel.z));
                }
            }
        }
    }
}