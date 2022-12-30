#include "framebuffer.h"
#include <iostream>
#include "../shader.h"
#include <vector>

namespace anim
{

    Framebuffer::Framebuffer(uint32_t width, uint32_t height, GLenum format, bool is_msaa)
        : width_(width), height_(height), format_(format), is_msaa_(is_msaa)
    {
        set_quad_VAO();
        if (!is_msaa_)
        {
        }
        else
        {
        }
        init_framebuffer();
    }

    Framebuffer::~Framebuffer()
    {
        glDeleteVertexArrays(1, &quad_VAO_);
        glDeleteBuffers(1, &quad_VBO_);
        glDeleteFramebuffers(1, &FBO_);
        glDeleteTextures(1, &screen_texture_id_);
        glDeleteRenderbuffers(1, &d24s8_RBO_);
        if (is_msaa_)
        {
            glDeleteFramebuffers(1, &intermediate_FBO_);
            glDeleteTextures(1, &msaa_texture_id_);
        }
    }

    uint32_t Framebuffer::get_fbo() const
    {
        return FBO_;
    }
    uint32_t Framebuffer::get_color_texture() const
    {
        return screen_texture_id_;
    }
    uint32_t Framebuffer::get_width() const
    {
        return width_;
    }
    uint32_t Framebuffer::get_height() const
    {
        return height_;
    }
    GLenum Framebuffer::get_format() const
    {
        return format_;
    }
    float Framebuffer::get_aspect() const
    {
        return static_cast<float>(width_) / static_cast<float>(height_);
    }

    void Framebuffer::draw(Shader &shader)
    {
        shader.use();
        glUniform1i(glGetUniformLocation(shader.get_id(), "screenTexture"), 0);
        shader.set_vec2("iResolution", glm::vec2(width_, height_));
        glBindVertexArray(quad_VAO_);
        glBindTexture(GL_TEXTURE_2D, screen_texture_id_);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    void Framebuffer::init_framebuffer()
    {
        glGenFramebuffers(1, &FBO_);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO_);

        attach_color_attachment_texture();
        attach_depth24_stencil8_RBO();
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
#ifndef NDEBUG
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
#endif
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::attach_color_attachment_texture()
    {
        glGenTextures(1, &screen_texture_id_);
        glBindTexture(GL_TEXTURE_2D, screen_texture_id_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, format_, width_, height_, 0, format_, GL_UNSIGNED_BYTE, nullptr);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screen_texture_id_, 0);

        std::vector<GLenum> draw_buffers = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
        uint32_t screen_texture2_id;
        glGenTextures(1, &screen_texture2_id);
        glBindTexture(GL_TEXTURE_2D, screen_texture2_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, width_, height_, 0, GL_RED_INTEGER, GL_INT, NULL);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, screen_texture2_id, 0);
        glDrawBuffers(draw_buffers.size(), draw_buffers.data());
    }

    void Framebuffer::attach_depth24_stencil8_RBO()
    {
        glGenRenderbuffers(1, &d24s8_RBO_);
        glBindRenderbuffer(GL_RENDERBUFFER, d24s8_RBO_);

        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width_, height_);

        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, d24s8_RBO_);
    }
    void Framebuffer::init_framebuffer_with_MSAA()
    {
    }

    void Framebuffer::set_quad_VAO()
    {
        glGenVertexArrays(1, &quad_VAO_);
        glGenBuffers(1, &quad_VBO_);
        glBindVertexArray(quad_VAO_);
        glBindBuffer(GL_ARRAY_BUFFER, quad_VBO_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices_), &quad_vertices_, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    }
    void Framebuffer::bind_without_clear()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, FBO_);
        glViewport(0, 0, width_, height_);
    }

    void Framebuffer::bind(const glm::vec4 &color)
    {
        bind_without_clear();
        glClearColor(color.x, color.y, color.z, color.a);

        glClear(GL_COLOR_BUFFER_BIT);
    }
    void Framebuffer::bind_with_depth(const glm::vec4 &color)
    {
        bind_without_clear();
        glClearColor(color.x, color.y, color.z, color.a);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    void Framebuffer::bind_with_depth_and_stencil(const glm::vec4 &color)
    {
        bind_without_clear();
        glClearColor(color.x, color.y, color.z, color.a);
        glEnable(GL_DEPTH_TEST);
        glStencilMask(~0); // https://community.khronos.org/t/how-to-clear-stencil-buffer-after-stencil-test/15882/4
        glDisable(GL_SCISSOR_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glDisable(GL_STENCIL_TEST);
    }
    void Framebuffer::unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    bool Framebuffer::error()
    {
        return is_error_;
    }
}