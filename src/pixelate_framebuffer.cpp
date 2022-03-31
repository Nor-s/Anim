#include "pixelate_framebuffer.h"
#include <glad/glad.h>

#include "glcpp/model.h"
#include "glcpp/framebuffer.h"
#include "glcpp/shader.h"

PixelateFramebuffer::PixelateFramebuffer(int width, int height)
{
    set_size(width, height);
}
PixelateFramebuffer::~PixelateFramebuffer()
{
    RGB_shader_.reset();
    pixelate_shader_.reset();
    tmp_shader_.reset();
    RGB_framebuffer_.reset();
    pixelate_framebuffer_.reset();
}
// model capture to RGB for blend
//(https://community.khronos.org/t/alpha-blending-issues-when-drawing-frame-buffer-into-default-buffer/73958)
//(https://stackoverflow.com/questions/2171085/opengl-blending-with-previous-contents-of-framebuffer/4076268)

void PixelateFramebuffer::pre_draw(std::shared_ptr<glcpp::Model> &model, glcpp::Shader &shader, glm::mat4 &view, glm::mat4 &projection)
{
    capture_rgb(model, shader, view, projection);
    capture_rgba(model, shader, view, projection);
}
void PixelateFramebuffer::draw()
{
    pixelate_framebuffer_->draw(*pixelate_shader_);
}
void PixelateFramebuffer::print_to_png(const std::string &file_name)
{
    pixelate_framebuffer_->print_color_texture(file_name);
}
void PixelateFramebuffer::capture_rgb(std::shared_ptr<glcpp::Model> &model, glcpp::Shader &shader, glm::mat4 &view, glm::mat4 &projection)
{
    glBindFramebuffer(GL_FRAMEBUFFER, RGB_framebuffer_->get_fbo());
    {
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glViewport(0, 0, RGB_framebuffer_->get_width(), RGB_framebuffer_->get_height());
        glClearColor(0.3f, 0.3f, 0.3f, 0.3f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        model->draw(shader, view, projection);
    }
    // RGB_framebuffer_->print_color_texture("RGB.png");
}

void PixelateFramebuffer::capture_rgba(std::shared_ptr<glcpp::Model> &model, glcpp::Shader &shader, glm::mat4 &view, glm::mat4 &projection)
{
    glBindFramebuffer(GL_FRAMEBUFFER, pixelate_framebuffer_->get_fbo());
    {
        glEnable(GL_STENCIL_TEST);
        glEnable(GL_DEPTH_TEST);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glViewport(0, 0, pixelate_framebuffer_->get_width(), pixelate_framebuffer_->get_height());
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);

        model->draw(*tmp_shader_, view, projection);

        glStencilFunc(GL_EQUAL, 1, 0xFF);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        RGB_framebuffer_->draw(*RGB_shader_);

        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);
    }
    // pixelate_framebuffer_->print_color_texture("RGBA.png");
}
void PixelateFramebuffer::set_RGB_shader(std::shared_ptr<glcpp::Shader> &shader)
{
    RGB_shader_ = shader;
}
void PixelateFramebuffer::set_pixelate_shader(std::shared_ptr<glcpp::Shader> &shader)
{
    pixelate_shader_ = shader;
}
void PixelateFramebuffer::set_tmp_shader(std::shared_ptr<glcpp::Shader> &shader)
{
    tmp_shader_ = shader;
}
void PixelateFramebuffer::set_size(int width, int height)
{
    width_ = width;
    height_ = height;
    pixelate_framebuffer_ = std::make_unique<glcpp::Framebuffer>(width_, height_, GL_RGBA);
    RGB_framebuffer_ = std::make_unique<glcpp::Framebuffer>(width_, height_, GL_RGB);
}

int PixelateFramebuffer::get_width()
{
    return width_;
}
uint32_t PixelateFramebuffer::get_texture()
{
    return pixelate_framebuffer_->get_color_texture();
}
