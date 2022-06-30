#ifndef PIXELATE_FRAMEBUFFER_H
#define PIXELATE_FRAMEBUFFER_H

#include <memory>
#include <glm/gtc/matrix_transform.hpp>
#include "glcpp/shader.h"

namespace glcpp
{
    class Framebuffer;
    class Model;
    class Entity;
}

class PixelateFramebuffer
{
public:
    PixelateFramebuffer(int width, int height);
    ~PixelateFramebuffer();
    void pre_draw(std::shared_ptr<glcpp::Entity> &entity, glcpp::Shader &shader, glm::mat4 &view, glm::mat4 &projection);
    void draw();
    void to_png(const char *save_path);
    void set_pixelate_shader(std::shared_ptr<glcpp::Shader> &shader);
    void set_tmp_shader(std::shared_ptr<glcpp::Shader> &shader);
    void set_outline_shader(std::shared_ptr<glcpp::Shader> &shader);
    void set_outline_flag(bool flag);
    void set_outline_color(const glm::vec3 &color);
    void set_size(int width, int height);
    int get_width();
    uint32_t get_texture();
    int get_factor();
    void set_factor(int factor);
    glcpp::Framebuffer &get_framebuffer();
    glcpp::Framebuffer &get_outline_framebuffer();
    glm::vec3 &get_outline_color();

private:
    void capture_rgba(std::shared_ptr<glcpp::Entity> &entity, glcpp::Shader &shader, glm::mat4 &view, glm::mat4 &projection);
    void capture_outline();

private:
    std::shared_ptr<glcpp::Shader> pixelate_shader_;
    std::shared_ptr<glcpp::Shader> outline_shader_;
    std::shared_ptr<glcpp::Shader> tmp_shader_;
    std::unique_ptr<glcpp::Framebuffer> pixelate_framebuffer_; // pass 1
    std::unique_ptr<glcpp::Framebuffer> outline_framebuffer_;  // pass 2
    int width_;
    int height_;
    int factor_;
    // https://www.seniorcare2share.com/should-you-use-a-black-outline-in-pixel-art/
    bool is_outline_ = true;
    glm::vec3 outline_color_{0.0f, 0.0f, 0.0f};
};

#endif