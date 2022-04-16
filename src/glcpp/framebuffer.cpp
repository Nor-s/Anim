#include "framebuffer.h"

#include <iostream>
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

#include "shader.h"

namespace glcpp
{

    Framebuffer::Framebuffer(uint32_t width, uint32_t height, GLenum format)
        : width_(width), height_(height), format_(format)
    {
        create_framebuffer();
        attach_color_attachment_texture();
        attach_depth24_stencil8_RBO();
        set_quad_VAO();
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    Framebuffer::~Framebuffer()
    {
        glDeleteVertexArrays(1, &quad_VAO_);
        glDeleteBuffers(1, &quad_VBO_);
        glDeleteFramebuffers(1, &FBO_);
        glDeleteTextures(1, &color_texture_id_);
        glDeleteRenderbuffers(1, &d24s8_RBO_);
    }

    uint32_t Framebuffer::get_fbo() const
    {
        return FBO_;
    }
    uint32_t Framebuffer::get_color_texture() const
    {
        return color_texture_id_;
    }
    uint32_t Framebuffer::get_width() const
    {
        return width_;
    }
    uint32_t Framebuffer::get_height() const
    {
        return height_;
    }
    float Framebuffer::get_aspect() const
    {
        return static_cast<float>(width_) / static_cast<float>(height_);
    }

    void Framebuffer::draw(Shader &shader)
    {
        shader.use();
        glUniform1i(glGetUniformLocation(shader.ID, "texture_diffuse1"), 0);
        shader.setVec2("iResolution", glm::vec2(width_, height_));
        glBindVertexArray(quad_VAO_);
        glBindTexture(GL_TEXTURE_2D, color_texture_id_);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    void Framebuffer::print_color_texture(std::string const &file_name, GLenum format)
    {
        if (format == GL_RED)
        {
            format = format_;
        }
        stbi_flip_vertically_on_write(true);
        // 출력하고자 하는 프레임 버퍼 바인딩
        glBindFramebuffer(GL_FRAMEBUFFER, FBO_);
        glViewport(0, 0, width_, height_);
        // 화소를 저장할 공간을 할당받음
        GLubyte *pixels = (GLubyte *)malloc(width_ * height_ * sizeof(GLubyte) * 4);
        // glReadPixel 로 현재 바인딩된 프레임버퍼의 화소값들을 pixels에 저장
        glReadPixels(0, 0, width_, height_, format, GL_UNSIGNED_BYTE, pixels);

        // 오픈소스 stb 라이브러리를 사용하여 받아온 화소값들을 png 포맷으로 변환한다.
        if (format == GL_RGBA)
            stbi_write_png(file_name.c_str(), width_, height_, 4, pixels, 0);
        else if (format == GL_RGB)
            stbi_write_png(file_name.c_str(), width_, height_, 3, pixels, 0);
    }

    void Framebuffer::create_framebuffer()
    {
        glGenFramebuffers(1, &FBO_);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO_);
    }

    void Framebuffer::attach_color_attachment_texture()
    {
        glGenTextures(1, &color_texture_id_);
        glBindTexture(GL_TEXTURE_2D, color_texture_id_);

        glTexImage2D(GL_TEXTURE_2D, 0, format_, width_, height_, 0, format_, GL_UNSIGNED_BYTE, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture_id_, 0);
    }

    void Framebuffer::attach_depth24_stencil8_RBO()
    {
        glGenRenderbuffers(1, &d24s8_RBO_);
        glBindRenderbuffer(GL_RENDERBUFFER, d24s8_RBO_);

        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width_, height_);

        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, d24s8_RBO_);
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

    void Framebuffer::bind()
    {
        bind_without_clear();
        glClear(GL_COLOR_BUFFER_BIT);
    }
    void Framebuffer::bind_with_depth()
    {
        bind_without_clear();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    void Framebuffer::bind_with_depth_and_stencil()
    {
        bind_without_clear();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }
    void Framebuffer::unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}