#ifndef GLCPP_FRAMEBUFFER_H
#define GLCPP_FRAMEBUFFER_H

#include <glad/glad.h>
#include <iostream>
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

#include "shader.h"

namespace glcpp
{

    class Framebuffer
    {
    public:
        Framebuffer(uint32_t width, uint32_t height, std::string const &vert_path, std::string const &frag_path)
            : width_(width), height_(height), pixelate_factor_(100), shader_(vert_path.c_str(), frag_path.c_str())
        {
            create_framebuffer();
            attach_color_attachment_texture();
            attach_depth24_stencil8_RBO();
            set_quad_VAO();
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            shader_.use();
            shader_.setInt("screenTexture", 0);
            shader_.setInt("pixelateFactor", pixelate_factor_);
        }

        virtual ~Framebuffer()
        {
            glDeleteFramebuffers(1, &FBO_);
        }
        void set_pixelate_factor(int factor)
        {
            pixelate_factor_ = factor;
        }

        uint32_t get_fbo() const
        {
            return FBO_;
        }
        uint32_t get_color_texture() const
        {
            return color_texture_id_;
        }
        uint32_t get_width() const
        {
            return width_;
        }
        uint32_t get_height() const
        {
            return height_;
        }
        Shader &get_shader()
        {
            return shader_;
        }

        void draw()
        {
            shader_.use();
            shader_.setInt("pixelateFactor", pixelate_factor_);
            glBindVertexArray(quad_VAO_);
            glBindTexture(GL_TEXTURE_2D, color_texture_id_);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        void print_color_texture(std::string const &file_name)
        {
        }

    private:
        void create_framebuffer()
        {
            glGenFramebuffers(1, &FBO_);
            glBindFramebuffer(GL_FRAMEBUFFER, FBO_);
        }

        void attach_color_attachment_texture()
        {
            glGenTextures(1, &color_texture_id_);
            glBindTexture(GL_TEXTURE_2D, color_texture_id_);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture_id_, 0);
        }

        void attach_depth24_stencil8_RBO()
        {
            glGenRenderbuffers(1, &d24s8_RBO_);
            glBindRenderbuffer(GL_RENDERBUFFER, d24s8_RBO_);

            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width_, height_);

            glBindRenderbuffer(GL_RENDERBUFFER, 0);

            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, d24s8_RBO_);
        }
        void set_quad_VAO()
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

    private:
        uint32_t FBO_;
        uint32_t color_texture_id_;
        uint32_t d24s8_RBO_;
        uint32_t quad_VAO_;
        uint32_t quad_VBO_;
        uint32_t width_;
        uint32_t height_;
        uint32_t pixelate_factor_;
        Shader shader_;
        float quad_vertices_[24] = {
            // positions   // texCoords
            -1.0f, 1.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 0.0f,

            -1.0f, 1.0f, 0.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f};
    };
}

#endif
