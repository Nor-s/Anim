#ifndef ANIM_GRAPHICS_OPENGL_FRAMEBUFFER_H
#define ANIM_GRAPHICS_OPENGL_FRAMEBUFFER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

namespace anim
{
    class Shader;
    class Framebuffer
    {
    public:
        Framebuffer(uint32_t width, uint32_t height, GLenum format = GL_RGB, bool is_msaa = false);
        virtual ~Framebuffer();
        uint32_t get_fbo() const;
        uint32_t get_color_texture() const;
        uint32_t get_width() const;
        uint32_t get_height() const;
        GLenum get_format() const;
        float get_aspect() const;
        void draw(Shader &shader);
        void bind_without_clear();
        void bind(const glm::vec4 &color = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
        void bind_with_depth(const glm::vec4 &color = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
        void bind_with_depth_and_stencil(const glm::vec4 &color = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
        void unbind();
        bool error();

    private:
        void init_framebuffer();
        void init_framebuffer_with_MSAA();
        void attach_color_attachment_texture();
        void attach_depth24_stencil8_RBO();
        void set_quad_VAO();

    private:
        uint32_t width_ = 0;
        uint32_t height_ = 0;
        GLenum format_ = GL_RGB;
        bool is_msaa_ = false;
        uint32_t FBO_ = 0;
        uint32_t intermediate_FBO_ = 0;
        uint32_t msaa_texture_id_ = 0;
        uint32_t screen_texture_id_ = 0;
        uint32_t d24s8_RBO_ = 0;
        uint32_t quad_VAO_ = 0;
        uint32_t quad_VBO_ = 0;
        int samples_ = 4;
        bool is_error_ = false;

        inline static const float quad_vertices_[24] = {
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
