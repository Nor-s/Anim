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
    };
    struct Texture
    {
        unsigned int id;
        std::string type;
        std::string path;
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