#include "pixelate_framebuffer.h"
#include <glad/glad.h>

#include "glcpp/model.h"
#include "glcpp/framebuffer.h"
#include "glcpp/shader.h"
#include "glcpp/entity.h"

PixelateFramebuffer::PixelateFramebuffer(uint32_t width, uint32_t height, uint32_t factor)
    : width_(width), height_(height), factor_(factor)
{
    set_framebuffer();
}

PixelateFramebuffer::~PixelateFramebuffer()
{
    pixelate_shader_.reset();
    outline_shader_.reset();
    pixelate_framebuffer_.reset();
    outline_framebuffer_.reset();
}

// model capture to RGB for blend
//(https://community.khronos.org/t/alpha-blending-issues-when-drawing-frame-buffer-into-default-buffer/73958)
//(https://stackoverflow.com/questions/2171085/opengl-blending-with-previous-contents-of-framebuffer/4076268)
void PixelateFramebuffer::pre_draw(std::shared_ptr<glcpp::Entity> &entity, glcpp::Shader &shader, glm::mat4 &view, glm::mat4 &projection)
{
    capture_rgba(entity, shader, view, projection);
    if (is_outline_)
    {
        capture_outline();
    }
}

void PixelateFramebuffer::draw()
{
    if (is_outline_)
    {
        outline_framebuffer_->draw(*pixelate_shader_);
    }
    else
    {
        pixelate_framebuffer_->draw(*pixelate_shader_);
    }
}

void PixelateFramebuffer::to_png(const char *save_path)
{
    if (is_outline_)
    {
    }
    else
    {
    }
}

void PixelateFramebuffer::capture_rgba(std::shared_ptr<glcpp::Entity> &entity, glcpp::Shader &shader, glm::mat4 &view, glm::mat4 &projection)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    pixelate_framebuffer_->bind_with_depth({0.0f, 0.0f, 0.0f, 0.0f});

    glViewport(0, 0, pixelate_framebuffer_->get_width(), pixelate_framebuffer_->get_height());
    entity->draw(shader, view, projection);
    pixelate_framebuffer_->unbind();
    glDisable(GL_DEPTH_TEST);
}

void PixelateFramebuffer::capture_outline()
{
    glDisable(GL_DEPTH_TEST);

    outline_framebuffer_->bind_with_depth({0.0f, 0.0f, 0.0f, 0.0f});
    outline_shader_->use();
    outline_shader_->set_vec3("outline_color", outline_color_);
    pixelate_framebuffer_->draw(*outline_shader_);
    outline_framebuffer_->unbind();
}

void PixelateFramebuffer::set_pixelate_shader(std::shared_ptr<glcpp::Shader> shader)
{
    pixelate_shader_ = std::move(shader);
}
void PixelateFramebuffer::set_outline_shader(std::shared_ptr<glcpp::Shader> shader)
{
    outline_shader_ = std::move(shader);
}
void PixelateFramebuffer::set_outline_flag(bool flag)
{
    is_outline_ = flag;
}
void PixelateFramebuffer::set_outline_color(const glm::vec3 &color)
{
    outline_color_ = color;
}
void PixelateFramebuffer::set_framebuffer(uint32_t width, uint32_t height)
{
    set_size(width, height);
    set_framebuffer();
}
void PixelateFramebuffer::set_framebuffer(uint32_t factor)
{
    set_factor(factor);
    set_framebuffer();
}
void PixelateFramebuffer::set_framebuffer()
{
    uint32_t width = width_ / factor_;
    uint32_t height = height_ / factor_;
    width = (width) ? width : 1u;
    height = (height) ? height : 1u;

    pixelate_framebuffer_.reset(new glcpp::Framebuffer(width, height, GL_RGBA));
    outline_framebuffer_.reset(new glcpp::Framebuffer(width, height, GL_RGBA));
}
void PixelateFramebuffer::set_size(uint32_t width, uint32_t height)
{
    width_ = width;
    height_ = height;
}
void PixelateFramebuffer::set_factor(uint32_t factor)
{
    factor_ = factor;
}
uint32_t PixelateFramebuffer::get_width()
{
    return width_;
}
uint32_t PixelateFramebuffer::get_texture()
{
    return outline_framebuffer_->get_color_texture(); // pixelate_framebuffer_->get_color_texture();
}
uint32_t PixelateFramebuffer::get_factor()
{
    return factor_;
}

glcpp::Framebuffer &PixelateFramebuffer::get_framebuffer()
{
    return *pixelate_framebuffer_;
}
glcpp::Framebuffer &PixelateFramebuffer::get_outline_framebuffer()
{
    return *outline_framebuffer_;
}
glm::vec3 &PixelateFramebuffer::get_outline_color()
{
    return outline_color_;
}
