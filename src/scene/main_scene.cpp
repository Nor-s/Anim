#include "main_scene.h"
#include "shared_resources.h"
#include "pixelate_framebuffer.h"
#include <glcpp/shader.h>
#include <glcpp/framebuffer.h>
#include <glcpp/camera.h>
#include <glcpp/model.h>
#include <glcpp/animator.h>
#include <glcpp/entity.h>
#include <glcpp/image.h>

#include <filesystem>

namespace fs = std::filesystem;

MainScene::MainScene(uint32_t width, uint32_t height, std::shared_ptr<SharedResources> resources)
{
    if (resources == nullptr)
    {
        resources_ = std::make_shared<SharedResources>();
    }
    else
    {
        resources_ = std::move(resources);
    }
    init_shader();
    init_framebuffer(width, height);
    init_camera();
    selected_entity_ = std::make_shared<glcpp::Entity>();
}

void MainScene::init_shader()
{
    resources_->add_shader("model", "./resources/shaders/1.model_loading.vs", "./resources/shaders/1.model_loading.fs");
    resources_->add_shader("animation", "./resources/shaders/animation_loading.vs", "./resources/shaders/1.model_loading.fs");
    resources_->add_shader("framebuffer", "./resources/shaders/simple_framebuffer.vs", "./resources/shaders/simple_framebuffer.fs");
    resources_->add_shader("blurred_framebuffer", "./resources/shaders/simple_framebuffer.vs", "./resources/shaders/skybox_blur.fs");
    resources_->add_shader("outline", "./resources/shaders/outline_framebuffer.vs", "./resources/shaders/outline_framebuffer.fs");
    resources_->add_shader("grid", "./resources/shaders/grid.vs", "./resources/shaders/grid.fs");
}

void MainScene::init_framebuffer(uint32_t width, uint32_t height)
{
    set_size(width, height);

    framebuffer_.reset(new glcpp::Framebuffer{width, height, GL_RGB, true});
    grid_framebuffer_.reset(new glcpp::Image{width, height, GL_RGBA});
}

void MainScene::init_pixelate_framebuffer(uint32_t width, uint32_t height)
{
    auto state = get_component<PixelateStateComponent>();
    pixelate_framebuffer_.reset(new PixelateFramebuffer(width, height, state->get_factor()));
    pixelate_framebuffer_->set_pixelate_shader(resources_->get_mutable_shader("framebuffer"));
    pixelate_framebuffer_->set_outline_shader(resources_->get_mutable_shader("outline"));
    pixelate_framebuffer_->set_outline_color(state->get_outline_color());
    pixelate_framebuffer_->set_outline_flag(state->get_is_outline());
}

void MainScene::init_camera()
{
    camera_ = std::make_shared<glcpp::Camera>(glm::vec3(0.0f, 100.0f, 500.0f));
}

void MainScene::pre_draw()
{
    update_framebuffer();
    camera_->set_view_and_projection(framebuffer_->get_aspect());
    glcpp::Shader *shader = shader = resources_->get_mutable_shader("model").get();
    if (selected_entity_ && selected_entity_->get_mutable_model() && selected_entity_->has_bone() && selected_entity_->has_animation_component())
    {
        shader = resources_->get_mutable_shader("animation").get();
        resources_->get_mutable_animator()->update_animation(delta_time_, selected_entity_.get(), shader);
    }
    shader->use();
    shader->set_vec3("viewPos", camera_->get_current_pos());
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
    glcpp::Shader *shader = resources_->get_mutable_shader("model").get();

    if (selected_entity_ && selected_entity_->get_mutable_model() && selected_entity_->has_bone() && selected_entity_->has_animation_component())
    {
        shader = resources_->get_mutable_shader("animation").get();
    }
    glEnable(GL_CULL_FACE);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    framebuffer_->bind_with_depth(background_color_);
    {
        grid_shader->use();
        grid_shader->set_mat4("view", camera_->get_view());
        grid_shader->set_mat4("projection", camera_->get_projection());

        if (selected_entity_)
        {
            selected_entity_->draw(*shader, camera_->get_view(), camera_->get_projection());
        }
        grid_framebuffer_->draw(*grid_shader);
    }
    framebuffer_->unbind();
#ifndef NDEBUG

    auto error = glGetError();
    if (error != GL_NO_ERROR)
    {
        std::cout << "main scene: " << error << std::endl;
    }
    std::cout << camera_->get_current_pos().y << "\n";
    std::cout << camera_->get_projection()[3][1] << "\n";
    // auto cmwvc =
    // auto unpp = glm::inverse(camera_->get_view()) * glm::inverse(camera_->get_projection()) * glm::vec4(cm.x, cm.y, 1.0f, 1.0f);
    // unpp.x /= unpp.w;
    // unpp.y /= unpp.w;
    // unpp.z /= unpp.w;
    // std::cout << unpp.x << " " << unpp.y << " " << unpp.z << "\n";
#endif
}

void MainScene::draw()
{
    auto framebuffer_shader = resources_->get_mutable_shader("framebuffer");

    framebuffer_->draw(*framebuffer_shader);
}

glcpp::Entity *MainScene::get_mutable_selected_entity()
{
    return selected_entity_.get();
}