#ifndef ANIM_GRAPHICS_OPENGL_GL_MESH_H
#define ANIM_GRAPHICS_OPENGL_GL_MESH_H

#include "../mesh.h"
#include <memory>

namespace anim::gl
{
// TODO: Refactor Create function
std::unique_ptr<Mesh> CreateBiPyramid();
class GLMesh : public anim::Mesh
{
public:
	GLMesh(std::string_view mesh_name,
		   const std::vector<Vertex>& vertices,
		   const std::vector<unsigned int>& indices,
		   const std::vector<Texture>& textures,
		   const MaterialProperties& mat_properties);
	GLMesh(std::string_view mesh_name, const std::vector<Vertex>& vertices);
	~GLMesh();
	void draw(anim::Shader& shader) override;
	void draw_outline(anim::Shader& shader) override;
	virtual void init_morph(size_t location, const MorphTargetDeltas* morph_deltas) override;
	virtual void set_morph_weight(size_t location, float weight) override;

private:
	void init_buffer();
	void draw();

private:
	unsigned int VAO_, VBO_, EBO_;
	unsigned int MORPH_VBO_[3] = {0, 0, 0};
	float weights_[3] = {0.0f, 0.0f, 0.0f};
};
}	 // namespace anim::gl
#endif