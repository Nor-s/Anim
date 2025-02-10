#ifndef ANIM_ANIMATION_MORPHTARGET_H
#define ANIM_ANIMATION_MORPHTARGET_H

#include <glm/glm.hpp>
#include <glm/glm/vec3.hpp>
#include <assimp/scene.h>
#include <memory>
#include <vector>

namespace anim
{

class Mesh;

struct MorphTargetDeltas
{
	/** change in position */
	std::vector<glm::vec3> position_delta;
	/** Tangent basis normal */
	std::vector<glm::vec3> tangent_z_delta;
};

class MorphTarget
{
	friend std::unique_ptr<MorphTarget> CreateMorphTarget(const aiScene* scene, const aiString target_name);

public:
	MorphTarget();
	~MorphTarget();

	const std::string& get_name() const
	{
		return name_;
	}
	const std::vector<std::string>& get_mesh_names() const
	{
		return mesh_names_;
	}
	const MorphTargetDeltas* get_morph_delta(uint32_t idx) const
	{
		if (morph_target_deltas_.size() <= idx)
		{
			return nullptr;
		}
		return &morph_target_deltas_[idx];
	}
	void reset_mesh()
	{
		meshes_.clear();
	}
	void push_back(std::shared_ptr<Mesh>& mesh)
	{
		meshes_.emplace_back(mesh);
	}
	const Mesh* get_mesh(size_t idx)
	{
		if (meshes_.size() <= idx)
		{
			return nullptr;
		}
		return meshes_[idx].get();
	}
	const size_t get_count() const
	{
		return meshes_.size();
	}

private:
	std::string name_;
	std::vector<std::string> mesh_names_;
	std::vector<MorphTargetDeltas> morph_target_deltas_;
	std::vector<std::shared_ptr<Mesh>> meshes_;
};

std::unique_ptr<MorphTarget> CreateMorphTarget(const aiScene* scene, const aiString target_name);
std::vector<std::shared_ptr<MorphTarget>> CreateMorphTargets(const aiScene*);

}	 // namespace anim

#endif
