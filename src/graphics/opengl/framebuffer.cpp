#include "framebuffer.h"
#include <iostream>
#include "../shader.h"

namespace anim
{
    namespace util
    {
        GLenum GetTextureTarget(bool is_msaa)
        {
            return is_msaa ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
        }

        GLenum ConvertFormat(const FramebufferTextureFormat &format)
        {
            switch (format)
            {
            case FramebufferTextureFormat::RGBA8:
                return GL_RGB;
            case FramebufferTextureFormat::RGB:
                return GL_RGB;
            case FramebufferTextureFormat::RED_INTEGER:
                return GL_RED_INTEGER;
            case FramebufferTextureFormat::DEPTH24STENCIL8:
                return GL_DEPTH24_STENCIL8;
            default:
                return GL_RGBA;
            }
        }

        GLenum ConvertInternalFormat(const FramebufferTextureFormat &format)
        {
            switch (format)
            {
            case FramebufferTextureFormat::RGBA8:
                return GL_RGBA8;
            case FramebufferTextureFormat::RGB:
                return GL_RGB;
            case FramebufferTextureFormat::RED_INTEGER:
                return GL_R32I;
            case FramebufferTextureFormat::DEPTH24STENCIL8:
                return GL_DEPTH24_STENCIL8;
            default:
                return GL_RGBA;
            }
        }

        GLenum GetDataType(const FramebufferTextureFormat &format)
        {
            switch (format)
            {
            case FramebufferTextureFormat::RED_INTEGER:
                return GL_INT;
            default:
                return GL_UNSIGNED_BYTE;
            }
        }

        void AttachColorAttachment(uint32_t &id, int samples, uint32_t width, uint32_t height, const FramebufferTextureSpec &spec, int index)
        {
            bool is_msaa = samples > 1;
            GLenum format = ConvertFormat(spec.texture_format);
            GLenum internal_format = ConvertInternalFormat(spec.texture_format);
            GLenum texture_target = GetTextureTarget(is_msaa);

            glGenTextures(1, &id);
            glBindTexture(texture_target, id);
            if (is_msaa)
            {
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internal_format, width, height, GL_TRUE);
            }
            else
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);
                // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width_, height_, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
                glGenerateMipmap(GL_TEXTURE_2D);
            }
            glBindTexture(texture_target, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, texture_target, id, 0);
        }

        void AttachDepthRBO(uint32_t &id, int samples, uint32_t width, uint32_t height, const FramebufferTextureSpec &spec)
        {
            bool is_msaa = samples > 1;
            GLenum format = ConvertFormat(spec.texture_format);

            glGenRenderbuffers(1, &id);
            glBindRenderbuffer(GL_RENDERBUFFER, id);
            if (is_msaa)
            {
                glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, format, width, height);
            }
            else
            {
                glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
            }

            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, id);
        }

        bool GenFramebuffer(uint32_t &id, const FramebufferSpec &spec, std::vector<uint32_t> &color_id, uint32_t &depth_id)
        {
            bool is_error = false;
            glGenFramebuffers(1, &id);
            glBindFramebuffer(GL_FRAMEBUFFER, id);
            color_id.clear();
            color_id.resize(spec.color_attachments_spec.size());
            std::vector<GLenum> draw_buffers;
            for (int i = 0; i < spec.color_attachments_spec.size(); i++)
            {
                AttachColorAttachment(color_id[i], spec.samples, spec.width, spec.height, spec.color_attachments_spec[i], i);
                draw_buffers.push_back(GL_COLOR_ATTACHMENT0 + i);
            }
            if (draw_buffers.size() > 0)
            {
                glDrawBuffers(draw_buffers.size(), draw_buffers.data());
            }
            else
            {
                glDrawBuffer(GL_NONE);
            }
            if (spec.depth_attachment_spec.texture_format != FramebufferTextureFormat::None)
            {
                AttachDepthRBO(depth_id, spec.samples, spec.width, spec.height, spec.depth_attachment_spec);
            }

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
#ifndef NDEBUG
                std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
#endif
                is_error = false;
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            return is_error;
        }
    }

    Framebuffer::Framebuffer(const FramebufferSpec &spec)
        : spec_(spec)
    {
        set_quad_VAO();
        init_framebuffer();
    }

    Framebuffer::~Framebuffer()
    {
        glDeleteVertexArrays(1, &quad_VAO_);
        glDeleteBuffers(1, &quad_VBO_);
        glDeleteFramebuffers(1, &renderer_id_);
        glDeleteTextures(1, color_id_.data());
        glDeleteRenderbuffers(1, &depth_id_);

        if (spec_.samples > 1)
        {
            glDeleteFramebuffers(1, &intermediate_renderer_id_);
            glDeleteTextures(1, &intermediate_color_id_);
        }
    }

    uint32_t Framebuffer::get_fbo() const
    {
        return renderer_id_;
    }
    uint32_t Framebuffer::get_color_attachment(uint32_t index) const
    {
        return screen_texture_id_;
    }
    uint32_t Framebuffer::get_width() const
    {
        return spec_.width;
    }
    uint32_t Framebuffer::get_height() const
    {
        return spec_.height;
    }
    GLenum Framebuffer::get_color_format(uint32_t idx) const
    {
        return util::ConvertFormat(spec_.color_attachments_spec[idx].texture_format);
    }
    float Framebuffer::get_aspect() const
    {
        return static_cast<float>(spec_.width) / static_cast<float>(spec_.height);
    }

    void Framebuffer::draw(Shader &shader)
    {
        shader.use();
        glUniform1i(glGetUniformLocation(shader.get_id(), "screenTexture"), 0);
        shader.set_vec2("iResolution", glm::vec2(spec_.width, spec_.height));
        glBindVertexArray(quad_VAO_);
        glBindTexture(GL_TEXTURE_2D, screen_texture_id_);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    void Framebuffer::init_framebuffer_with_MSAA()
    {
        // configure MSAA framebuffer
        // --------------------------
        glGenFramebuffers(1, &renderer_id_);
        glBindFramebuffer(GL_FRAMEBUFFER, renderer_id_);
        // create a multisampled color attachment texture
        color_id_.resize(2);
        glGenTextures(1, &color_id_[0]);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, color_id_[0]);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, spec_.samples, GL_RGBA8, spec_.width, spec_.height, GL_TRUE);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, color_id_[0], 0);

        glGenTextures(1, &color_id_[1]);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, color_id_[1]);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, spec_.samples, GL_R32I, spec_.width, spec_.height, GL_TRUE);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D_MULTISAMPLE, color_id_[1], 0);

        std::vector<GLenum> draw_buffers;
        for (int i = 0; i < 2; i++)
        {
            draw_buffers.push_back(GL_COLOR_ATTACHMENT0 + i);
        }
        if (draw_buffers.size() > 0)
        {
            glDrawBuffers(draw_buffers.size(), draw_buffers.data());
        }
        // create a (also multisampled) renderbuffer object for depth and stencil attachments
        glGenRenderbuffers(1, &depth_id_);
        glBindRenderbuffer(GL_RENDERBUFFER, depth_id_);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, spec_.samples, GL_DEPTH24_STENCIL8, spec_.width, spec_.height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_id_);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
#ifndef NDEBUG
            std::cout << "ERROR::FRAMEBUFFER::MSAA Framebuffer is not complete!" << std::endl;
#endif
            is_error_ = true;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glGenFramebuffers(1, &intermediate_renderer_id_);
        glBindFramebuffer(GL_FRAMEBUFFER, intermediate_renderer_id_);
        // create a color attachment texture
        glGenTextures(1, &screen_texture_id_);
        glBindTexture(GL_TEXTURE_2D, screen_texture_id_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, spec_.width, spec_.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screen_texture_id_, 0); // we only need a color buffer

        uint32_t screen_texture_id_2;
        glGenTextures(1, &screen_texture_id_2);
        glBindTexture(GL_TEXTURE_2D, screen_texture_id_2);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, spec_.width, spec_.height, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, screen_texture_id_2, 0); // we only need a color buffer
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
#ifndef NDEBUG

            std::cout << "ERROR::FRAMEBUFFER:: Intermediate framebuffer is not complete!" << std::endl;
#endif
            is_error_ = true;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        intermediate_color_id_ = screen_texture_id_;
    }
    void Framebuffer::init_framebuffer()
    {
        if (spec_.samples > 1)
        {
            // std::vector<uint32_t> tmp_color;
            // uint32_t tmp_depth;
            // FramebufferSpec intermediate_spec(spec_.width, spec_.height, 1, {FramebufferTextureFormat::RGBA8});

            // is_error_ = is_error_ || util::GenFramebuffer(intermediate_renderer_id_, intermediate_spec, tmp_color, tmp_depth);

            // intermediate_color_id_ = tmp_color[0];
            // screen_texture_id_ = intermediate_color_id_;
            init_framebuffer_with_MSAA();
        }
        else
        {
            is_error_ = util::GenFramebuffer(renderer_id_, spec_, color_id_, depth_id_);
            screen_texture_id_ = color_id_[0];
        }
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
        glBindFramebuffer(GL_FRAMEBUFFER, renderer_id_);
        glViewport(0, 0, spec_.width, spec_.height);
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
        if (spec_.samples > 1)
        {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, renderer_id_);
            glReadBuffer(GL_COLOR_ATTACHMENT0);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediate_renderer_id_);
            glDrawBuffer(GL_COLOR_ATTACHMENT0);
            glBlitFramebuffer(0, 0, spec_.width, spec_.height, 0, 0, spec_.width, spec_.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

            glBindFramebuffer(GL_READ_FRAMEBUFFER, renderer_id_);
            glReadBuffer(GL_COLOR_ATTACHMENT1);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediate_renderer_id_);
            glDrawBuffer(GL_COLOR_ATTACHMENT1);
            glBlitFramebuffer(0, 0, spec_.width, spec_.height, 0, 0, spec_.width, spec_.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

            glReadBuffer(GL_NONE);
            glDrawBuffer(GL_NONE);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    bool Framebuffer::error()
    {
        return is_error_;
    }
}