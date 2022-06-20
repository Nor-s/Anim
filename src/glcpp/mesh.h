#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>

#define MAX_BONE_INFLUENCE 4

namespace glcpp
{
    class Shader;
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 tex_coords;
        // tangent
        glm::vec3 tangent;
        // bitangent
        glm::vec3 bitangent;
        // bone indexes which will influence this vertex
        int bone_ids[MAX_BONE_INFLUENCE];
        // weights from each bone
        float weights[MAX_BONE_INFLUENCE];

        Vertex()
            : tex_coords(glm::vec2(0.0f, 0.0f))
        {
            init_bone();
        }

        void init_bone()
        {
            for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
            {
                bone_ids[i] = -1;
                weights[i] = 0.0f;
            }
        }
        void set_bone(int boneId, float weight)
        {
            for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
            {
                if (bone_ids[i] < 0)
                {
                    bone_ids[i] = boneId;
                    weights[i] = weight;
                    break;
                }
            }
        }
        void set_position(const glm::vec3 &vec)
        {
            position = vec;
        }
        void set_normal(const glm::vec3 &vec)
        {
            normal = vec;
        }
        void set_texture_coords(const glm::vec2 &vec)
        {
            tex_coords = vec;
        }
        void set_tangent(const glm::vec3 &vec)
        {
            tangent = vec;
        }
        void set_bitangent(const glm::vec3 &vec)
        {
            bitangent = vec;
        }
    };
    struct Texture
    {
        unsigned int id{0};
        std::string type{""};
        std::string path{""};
    };

    class Mesh
    {

    public:
        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
        ~Mesh() = default;
        void draw(Shader &shader);

    private:
        std::vector<Vertex> vertices_;
        std::vector<unsigned int> indices_;
        std::vector<Texture> textures_;
        unsigned int VAO_, VBO_, EBO_;

        // setup vao, vbo, ebo => vertex attribute
        void setup_mesh();
    };
}

#endif