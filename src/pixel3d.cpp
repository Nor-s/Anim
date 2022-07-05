#include "pixel3D.h"

#include <stb/stb_image.h>
#include "glcpp/camera.h"
#include "glcpp/window.h"
#include "UI/main_layer.h"
#include "scene/scene.hpp"
#include "scene/main_scene.h"
#include "scene/shared_resources.h"
#include "glcpp/model.h"
#include "glcpp/entity.h"
#include "glcpp/component/animation_component.h"
#include "glcpp/exporter.h"

namespace fs = std::filesystem;

#ifndef NDEBUG

void error_callback(int code, const char *description)
{
    printf("error %d: %s\n", code, description);
}
#endif

Pixel3D::Pixel3D()
{
}

Pixel3D::~Pixel3D()
{
    scenes_.clear();
    ui_.reset();
}

void Pixel3D::init(uint32_t width, uint32_t height, const std::string &title)
{
    stbi_set_flip_vertically_on_load(true);
    init_window(width, height, title);
    init_ui();
    init_shared_resources();
    init_scene(width, height);
}
void Pixel3D::init_window(uint32_t width, uint32_t height, const std::string &title)
{
    window_ = std::make_unique<glcpp::Window>(width, height, title);
    window_->set_factor();
    window_->set_user_pointer(this);
    init_callback();
}
void Pixel3D::init_callback()
{
    //    window_->set_framebuffer_size_callback(framebuffer_size_callback);
    window_->set_scroll_callback(scroll_callback);
    window_->set_mouse_button_callback(mouse_btn_callback);
    window_->set_cursor_pos_callback(mouse_callback);

#ifndef NDEBUG

    glfwSetErrorCallback(error_callback);
#endif
}
void Pixel3D::init_ui()
{
    ui_ = std::make_unique<ui::MainLayer>();
    ui_->init(window_->get_handle());
}
void Pixel3D::init_shared_resources()
{
    shared_resources_ = std::make_shared<SharedResources>();
}
void Pixel3D::init_scene(uint32_t width, uint32_t height)
{
    scenes_.push_back(std::make_shared<MainScene>(width, height, shared_resources_));
}
void Pixel3D::loop()
{
    glfwSwapInterval(0);
    start_time_ = static_cast<float>(glfwGetTime());

    while (!window_->should_close())
    {
        update();

        pre_draw();
        {
            ui_->begin();

            ui_->draw_dock(fps_);

            this->draw_scene();

            auto entity = scenes_[current_scene_idx_]->get_mutable_selected_entity();

            ui_->draw_model_properties(scenes_[current_scene_idx_].get());

            ui_->draw_hierarchy_layer(entity);

            ui_->draw_timeline(scenes_[current_scene_idx_].get());

            ui_->end();
        }

        post_draw();
    }
}
void Pixel3D::update()
{
    update_window();
    update_time();
    update_resources();
    process_buttons();
}
void Pixel3D::update_window()
{
    auto size = window_->get_framebuffer_size();
    while (size.first == 0 || size.second == 0)
    {
        window_->wait_events();
        size = window_->get_framebuffer_size();
    }
    window_->set_size(size.first, size.second);
}
void Pixel3D::update_time()
{
    auto &ui_context = ui_->get_context();
    auto &time_context = ui_context.timeline_context;
    float current_time = static_cast<float>(glfwGetTime());
    frames_++;
    if (current_time - start_time_ >= 1.0)
    {
        fps_ = static_cast<float>(frames_) / (current_time - start_time_);
        frames_ = 0;
        start_time_ = current_time;
    }
    delta_frame_ = current_time - last_frame_;
    last_frame_ = current_time;
    for (auto &scene : scenes_)
    {
        if (ui_context.timeline_context.is_clicked_play_all)
        {
            scene->set_delta_time(-delta_frame_);
        }
        else
        {
            scene->set_delta_time(delta_frame_);
        }
    }
    auto entity = scenes_[current_scene_idx_]->get_mutable_selected_entity();
    if (entity->has_animation_component())
    {
        auto animation_component = entity->get_mutable_pointer_animation_component();
        animation_component->set_custom_tick_per_second(time_context.tps);
        animation_component->set_fps(time_context.fps);

        if (time_context.is_clicked_play)
        {
            animation_component->play();
        }
        if (time_context.is_clicked_stop)
        {
            animation_component->stop();
        }
    }
}
void Pixel3D::update_resources()
{
    glcpp::Exporter exporter;
    auto &shared_resources = scenes_[current_scene_idx_]->get_mutable_ref_shared_resources();
    auto entity = scenes_[current_scene_idx_]->get_mutable_selected_entity();
    auto &ui_context = ui_->get_context();
    auto &menu_context = ui_context.menu_context;
    auto &time_context = ui_context.timeline_context;
    auto &properties_context = ui_context.properties_context;
    if (menu_context.clicked_import_model)
    {
        std::pair<bool, bool> result = shared_resources->add_model_or_animation_by_path(menu_context.path.c_str());
        if (result.first)
        {
#ifndef NDEBUG
            std::cout << "import model\n";
#endif
            auto model = shared_resources->back_mutable_model();
            entity->set_model(model, shared_resources->get_models_size() - 1);
            exporter.to_json(model.get(), "model.json");
        }

        if (result.second)
        {
#ifndef NDEBUG
            std::cout << "import animation\n";
#endif
            auto animation = shared_resources->back_mutable_animation();
            entity->set_animation_component(animation, shared_resources_->get_animations_size() - 1);
        }
    }
    else
    {
        int current_animation_idx = entity->get_animation_id();
        int animation_idx[2] = {properties_context.animation_idx, time_context.animation_idx};
        for (int i = 0; i < 2; i++)
        {
            if (animation_idx[i] >= 0 && animation_idx[i] != current_animation_idx)
            {
#ifndef NDEBUG
                std::cout << "animation id: " << animation_idx[i] << " <-> " << entity->get_model_id() << std::endl;
#endif
                auto animation = shared_resources->get_mutable_animation(animation_idx[i]);
                entity->set_animation_component(animation, animation_idx[i]);
                break;
            }
        }
        if (properties_context.model_idx >= 0 && properties_context.model_idx != entity->get_model_id())
        {
#ifndef NDEBUG
            std::cout << "model id: " << properties_context.model_idx << " <-> " << entity->get_model_id() << std::endl;
#endif
            auto model = shared_resources->get_mutable_model(properties_context.model_idx);
            entity->set_model(model, properties_context.model_idx);
            exporter.to_json(model.get(), "model.json");
        }
    }
    if (menu_context.clicked_export_animation)
    {
    }
}
void Pixel3D::process_buttons()
{
    auto &time_context = ui_->get_context().timeline_context;
    if (time_context.is_clicked_mp2mm)
    {
        mp2mm_.open();
    }
}
void Pixel3D::pre_draw()
{
    process_input(window_->get_handle());

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, window_->get_width(), window_->get_height());
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void Pixel3D::draw_scene()
{
    size_t size = scenes_.size();
    for (size_t i = 0; i < size; i++)
    {
        std::string scene_name = std::string("scene") + std::to_string(i + 1);
        scenes_[i]->pre_draw();
        ui_->draw_scene(scene_name, scenes_[i].get());
        if (ui_->is_scene_layer_hovered(scene_name))
        {
            current_scene_idx_ = i;
        }
    }
}
void Pixel3D::post_draw()
{
    glfwSwapBuffers(window_->get_handle());

    glfwPollEvents();
}

void Pixel3D::process_input(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        scenes_[current_scene_idx_]->get_mutable_ref_camera()->process_keyboard(glcpp::FORWARD, delta_frame_);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        scenes_[current_scene_idx_]->get_mutable_ref_camera()->process_keyboard(glcpp::BACKWARD, delta_frame_);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        scenes_[current_scene_idx_]->get_mutable_ref_camera()->process_keyboard(glcpp::LEFT, delta_frame_);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        scenes_[current_scene_idx_]->get_mutable_ref_camera()->process_keyboard(glcpp::RIGHT, delta_frame_);
}

void Pixel3D::mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    auto app = static_cast<Pixel3D *>(glfwGetWindowUserPointer(window));
    if (app->is_pressed_ && app->scenes_.size() > app->current_scene_idx_)
    {
        app->scenes_[app->current_scene_idx_]->get_mutable_ref_camera()->process_mouse_movement((static_cast<float>(yposIn) - app->prev_mouse_.y) / 3.6f, (static_cast<float>(xposIn) - app->prev_mouse_.x) / 3.6f);
        app->prev_mouse_.x = xposIn;
        app->prev_mouse_.y = yposIn;
    }
    if (app->is_pressed_scroll_ && app->scenes_.size() > app->current_scene_idx_)
    {
        app->scenes_[app->current_scene_idx_]->get_mutable_ref_camera()->process_mouse_scroll_press((static_cast<float>(yposIn) - app->prev_mouse_.y), (static_cast<float>(xposIn) - app->prev_mouse_.x), app->delta_frame_);
        app->prev_mouse_.x = xposIn;
        app->prev_mouse_.y = yposIn;
    }
    app->cur_mouse_.x = xposIn;
    app->cur_mouse_.y = yposIn;
}

void Pixel3D::mouse_btn_callback(GLFWwindow *window, int button, int action, int mods)
{
    auto app = static_cast<Pixel3D *>(glfwGetWindowUserPointer(window));
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && app->ui_ && app->ui_->is_scene_layer_hovered("scene" + std::to_string(app->current_scene_idx_ + 1)))
    {
        app->prev_mouse_.x = app->cur_mouse_.x;
        app->prev_mouse_.y = app->cur_mouse_.y;
        app->is_pressed_ = true;
    }
    else
    {
        app->is_pressed_ = false;
    }
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS && app->ui_ && app->ui_->is_scene_layer_hovered("scene" + std::to_string(app->current_scene_idx_ + 1)))
    {
        app->prev_mouse_.x = app->cur_mouse_.x;
        app->prev_mouse_.y = app->cur_mouse_.y;
        app->is_pressed_scroll_ = true;
    }
    else
    {
        app->is_pressed_scroll_ = false;
    }
}

void Pixel3D::scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    auto app = static_cast<Pixel3D *>(glfwGetWindowUserPointer(window));
    if (app->scenes_.size() > app->current_scene_idx_ && app->ui_ && app->ui_->is_scene_layer_hovered("scene" + std::to_string(app->current_scene_idx_ + 1)))
        app->scenes_[app->current_scene_idx_]->get_mutable_ref_camera()->process_mouse_scroll(yoffset);
}
