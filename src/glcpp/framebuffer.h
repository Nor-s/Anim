#ifndef GLCPP_FRAMEBUFFER_H
#define GLCPP_FRAMEBUFFER_H

#include <glad/glad.h>

#include <string>

namespace glcpp
{
    class Shader;
    class Framebuffer
    {
    public:
        Framebuffer(uint32_t width, uint32_t height, GLenum format = GL_RGB);
        virtual ~Framebuffer();
        void set_pixelate_factor(int factor);
        uint32_t get_fbo() const;
        uint32_t get_color_texture() const;
        uint32_t get_width() const;
        uint32_t get_height() const;
        void draw(Shader &shader);
        void print_color_texture(const std::string &file_name);

    private:
        void create_framebuffer();
        void attach_color_attachment_texture();
        void attach_depth24_stencil8_RBO();
        void set_quad_VAO();

    private:
        uint32_t FBO_ = 0;
        uint32_t color_texture_id_ = 0;
        uint32_t d24s8_RBO_ = 0;
        uint32_t quad_VAO_ = 0;
        uint32_t quad_VBO_ = 0;
        uint32_t width_;
        uint32_t height_;
        uint32_t pixelate_factor_;
        GLenum format_;
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
