#ifndef GLCPP_TEXTURE_COMPONENT_H
#define GLCPP_TEXTURE_COMPONENT_H

#include <glad/glad.h>

#include <string>

#include "stb/stb_image.h"

namespace glcpp
{
    class Texture
    {
    public:
        Texture(GLenum texture_target, const std::string &file_name)
            : texture_target_(texture_target), file_name_(file_name)
        {
        }

        // Should be called once to load the texture
        bool load()
        {
            bool ret = false;
            unsigned char *image_data = stbi_load(file_name_.c_str(), &width_, &height_, &bpp_, 0);

            glGenTextures(1, &texture_id_);
            glBindTexture(texture_target_, texture_id_);

            if (texture_target_ == GL_TEXTURE_2D)
            {
                if (load_texture2d(image_data))
                {
                    ret = true;
                }
            }
            glBindTexture(texture_target_, 0);
            stbi_image_free(image_data);

            return ret;
        }

        // Must be called at least once for the specific texture unit
        void bind(GLenum texture_unit)
        {
            glActiveTexture(texture_unit);
            glBindTexture(texture_target_, texture_id_);
        }

    private:
        bool load_texture2d(unsigned char *image_data)
        {
            GLenum format;
            if (image_data)
            {
                if (bpp_ == 1)
                    format = GL_RED;
                else if (bpp_ == 3)
                    format = GL_RGB;
                else if (bpp_ == 4)
                    format = GL_RGBA;

                glTexImage2D(texture_target_, 0, format, width_, height_, 0, format, GL_UNSIGNED_BYTE, image_data);
                glGenerateMipmap(texture_target_);

                // for model loading
                glTexParameteri(texture_target_, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(texture_target_, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(texture_target_, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(texture_target_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                return true;
            }
            printf("Can't load texture from '%s' - %s\n", file_name_.c_str(), stbi_failure_reason());
            return false;
        }

    private:
        GLenum texture_target_;
        std::string file_name_;
        GLuint texture_id_;
        int width_;
        int height_;
        int bpp_;
    };
}

#endif /* TEXTURE_H */