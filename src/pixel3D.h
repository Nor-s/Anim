#ifndef SRC_PIXEL3D_H
#define SRC_PIXEL3D_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include "glcpp/application.hpp"

struct GLFWwindow;
class Scene;
class SharedResources;

namespace ui
{
    class MainLayer;
}

class Pixel3D : public glcpp::Application<Pixel3D>
{
    template <typename T>
    friend class glcpp::Application;

private:
    Pixel3D();

public:
    virtual ~Pixel3D();
    virtual void init(uint32_t width, uint32_t height, const std::string &title) override;
    void init_window(uint32_t width, uint32_t height, const std::string &title);
    void init_callback();
    void init_ui();
    void init_shared_resources();
    void init_scene(uint32_t width, uint32_t height);

    virtual void loop() override;
    void update();
    void update_time();
    void update_resources();
    void pre_draw();
    void draw_scene();
    void post_draw();
    void process_input(GLFWwindow *window);

    static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
    static void mouse_callback(GLFWwindow *window, double xposIn, double yposIn);
    static void mouse_btn_callback(GLFWwindow *window, int button, int action, int mods);
    static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

private:
    // timing
    float start_time_ = 0.0f;
    float delta_frame_ = 0.0f;
    float last_frame_ = 0.0f;
    float fps_ = 0.0f;
    int frames_ = 0;
    // mouse event
    bool is_pressed_ = false;
    bool is_pressed_scroll_ = false;
    glm::vec2 prev_mouse_{-1.0f, -1.0f}, cur_mouse_{-1.0f, -1.0f};
    std::vector<std::shared_ptr<Scene>> scenes_;
    std::shared_ptr<SharedResources> shared_resources_;
    std::unique_ptr<ui::MainLayer> ui_;
    uint32_t current_scene_idx_ = 0u;
};

#endif
