#ifndef ANIM_ENTITY_COMPONENT_MORPH_TARGET_COMPONENT_H
#define ANIM_ENTITY_COMPONENT_MORPH_TARGET_COMPONENT_H

#include "../component.h"

#include <string>
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <deque>

namespace anim
{
class Entity;
class MorphTarget;
class SharedResources;
class Mesh;
class Shader;

// weight, id
using MorphDatas = std::vector<std::pair<float, int>>;

struct MorphInfo
{
	size_t morph_target_index;
	size_t location;
};

class MorphTargetComponent : public ComponentBase<MorphTargetComponent>
{
	friend class SharedResources;

public:
	const std::vector<std::shared_ptr<MorphTarget>>& get_morph_targets()
	{
		return morph_targets_;
	}
	const std::vector<float>& get_weight()
	{
		return weights_;
	}
	void update_morph_datas(MorphDatas& datas, size_t count);
	void set_shader(Shader& shader);
	bool is_active(int index);
	const std::string& get_name(int index); 

protected:
	void on_changed(size_t a_idx, float a_weight);

	void set_morph_targets(std::vector<std::shared_ptr<MorphTarget>>&& a_morph_targets)
	{
		morph_targets_ = std::move(a_morph_targets);
		weights_.clear();
		weights_.resize(morph_targets_.size(), 0.0f);
		is_active_.resize(morph_targets_.size(), 0.0f);
	}
	void set_zero(MorphInfo& info);
	void set_weight(MorphInfo& info, float weight, bool isNeedInit);

private:
	std::vector<std::shared_ptr<MorphTarget>> morph_targets_;
	std::vector<float> weights_;
	std::vector<int> is_active_;
	std::deque<MorphInfo> active_morph_target_indexes_;
};
}	 // namespace anim

#endif