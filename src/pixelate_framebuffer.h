#ifndef PIXELATE_FRAMEBUFFER_H
#define PIXELATE_FRAMEBUFFER_H

#include <memory>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.h"

namespace glcpp
{
    class Framebuffer;
    class Model;
}

class PixelateFramebuffer
{
public:
    PixelateFramebuffer(int width, int height);
    ~PixelateFramebuffer();
    void pre_draw(std::shared_ptr<glcpp::Model> &model, glcpp::Shader &shader, glm::mat4 &view, glm::mat4 &projection);
    void draw();
    void set_RGB_shader(std::shared_ptr<glcpp::Shader> &shader);
    void set_pixelate_shader(std::shared_ptr<glcpp::Shader> &shader);
    void set_tmp_shader(std::shared_ptr<glcpp::Shader> &shader);
    void set_size(int width, int height);
    void print_to_png(const std::string &file_name);
    int get_width();
    uint32_t get_texture();

private:
    void capture_rgb(std::shared_ptr<glcpp::Model> &model, glcpp::Shader &shader, glm::mat4 &view, glm::mat4 &projection);
    void capture_rgba(std::shared_ptr<glcpp::Model> &model, glcpp::Shader &shader, glm::mat4 &view, glm::mat4 &projection);

private:
    std::shared_ptr<glcpp::Shader> RGB_shader_;
    std::shared_ptr<glcpp::Shader> pixelate_shader_;
    std::shared_ptr<glcpp::Shader> tmp_shader_;
    std::unique_ptr<glcpp::Framebuffer> RGB_framebuffer_;
    std::unique_ptr<glcpp::Framebuffer> pixelate_framebuffer_;
    int width_;
    int height_;
};

#endif