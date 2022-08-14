#ifndef ANIM_GRAPHICS_OPENGL_GL_MESH_H
#define ANIM_GRAPHICS_OPENGL_GL_MESH_H

#include "../mesh.h"

namespace anim::gl
{
    class GLMesh : public anim::Mesh
    {
    public:
        GLMesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices, const std::vector<Texture> &textures, const MaterialProperties &mat_properties);
        ~GLMesh();
        void draw(anim::Shader &shader) override;

    private:
        void init_buffer();

    private:
        unsigned int VAO_, VBO_, EBO_;
    };
}
#endif