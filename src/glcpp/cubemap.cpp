#include "cubemap.h"
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

namespace glcpp
{
    Cubemap::Cubemap(std::vector<std::string> faces, std::string const &vert_path, std::string const &frag_path)
        : faces_(faces), shader_(Shader(vert_path.c_str(), frag_path.c_str()))
    {
        load_cubemap();
        shader_.use();
        shader_.setInt("skybox", 0);
        set_VAO();
    }
    Cubemap::~Cubemap()
    {
        glDeleteVertexArrays(1, &VAO_);
        glDeleteBuffers(1, &VBO_);
    }

    void Cubemap::load_cubemap()
    {
        stbi_set_flip_vertically_on_load(false);
        glGenTextures(1, &texture_id_);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id_);

        int width, height, nrChannels;
        for (uint32_t i = 0; i < faces_.size(); i++)
        {
            unsigned char *data = stbi_load(faces_[i].c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);

            if (data)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                             0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            }
            else
            {
#ifndef NDEBUG

                std::cout << "Cubemap tex failed to load at path: " << faces_[i] << std::endl;
#endif
            }

            stbi_image_free(data);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        stbi_set_flip_vertically_on_load(true);
    }
    void Cubemap::draw(const glm::mat4 &view, const glm::mat4 &projection)
    {
        // draw skybox as last
        glDepthFunc(GL_LEQUAL); // change depth function so depth test passes when values are equal to depth buffer's content
        shader_.use();
        // remove translation from the view matrix
        shader_.setMat4("view", glm::mat4(glm::mat3(view)));
        shader_.setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(VAO_);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id_);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default
    }

    void Cubemap::set_VAO()
    {
        glGenVertexArrays(1, &VAO_);
        glGenBuffers(1, &VBO_);

        glBindVertexArray(VAO_);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_);

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_), &vertices_, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}