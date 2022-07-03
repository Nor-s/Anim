#ifndef PIXELATE_FRAMEBUFFER_H
#define PIXELATE_FRAMEBUFFER_H

#include <memory>
#include <glm/gtc/matrix_transform.hpp>
#include "glcpp/shader.h"
#include "glcpp/component/component.hpp"

namespace glcpp
{
    class Framebuffer;
    class Model;
    class Entity;
}

class PixelateStateComponent : public glcpp::ComponentBase<PixelateStateComponent>
{
public:
    bool *get_mutable_pointer_is_outline()
    {
        return &is_outline_;
    }
    uint32_t *get_mutable_pointer_factor()
    {
        return &factor_;
    }
    glm::vec3 *get_mutable_pointer_outline_color()
    {
        return &outline_color_;
    }
    bool get_is_outline() const
    {
        return is_outline_;
    }
    uint32_t get_factor() const
    {
        return factor_;
    }
    const glm::vec3 &get_outline_color() const
    {
        return outline_color_;
    }

private:
    bool is_outline_{true};
    uint32_t factor_{1u};
    glm::vec3 outline_color_{0.0f, 0.0f, 0.0f};
};

class PixelateFramebuffer
{
public:
    PixelateFramebuffer(uint32_t width, uint32_t height, uint32_t factor);
    ~PixelateFramebuffer();
    void pre_draw(std::shared_ptr<glcpp::Entity> &entity, glcpp::Shader &shader, glm::mat4 &view, glm::mat4 &projection);
    void draw();
    void to_png(const char *save_path);
    void set_pixelate_shader(std::shared_ptr<glcpp::Shader> shader);
    void set_outline_shader(std::shared_ptr<glcpp::Shader> shader);
    void set_outline_flag(bool flag);
    void set_outline_color(const glm::vec3 &color);
    void set_framebuffer(uint32_t width, uint32_t height);
    void set_framebuffer(uint32_t factor);
    void set_framebuffer();
    void set_size(uint32_t width, uint32_t height);
    void set_factor(uint32_t factor);

    uint32_t get_width();
    uint32_t get_texture();
    uint32_t get_factor();
    glcpp::Framebuffer &get_framebuffer();
    glcpp::Framebuffer &get_outline_framebuffer();
    glm::vec3 &get_outline_color();

private:
    void capture_rgba(std::shared_ptr<glcpp::Entity> &entity, glcpp::Shader &shader, glm::mat4 &view, glm::mat4 &projection);
    void capture_outline();

private:
    std::shared_ptr<glcpp::Shader> pixelate_shader_;
    std::shared_ptr<glcpp::Shader> outline_shader_;
    std::unique_ptr<glcpp::Framebuffer> pixelate_framebuffer_; // pass 1
    std::unique_ptr<glcpp::Framebuffer> outline_framebuffer_;  // pass 2
    uint32_t width_;
    uint32_t height_;
    uint32_t factor_;
    // https://www.seniorcare2share.com/should-you-use-a-black-outline-in-pixel-art/
    bool is_outline_ = true;
    glm::vec3 outline_color_{0.0f, 0.0f, 0.0f};
};

#endif