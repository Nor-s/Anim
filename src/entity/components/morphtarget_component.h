#ifndef ANIM_ENTITY_COMPONENT_MORPH_TARGET_COMPONENT_H
#define ANIM_ENTITY_COMPONENT_MORPH_TARGET_COMPONENT_H

#include "../component.h"

#include <string>
#include <memory>
#include <vector>
#include <deque>
#include <glm/glm.hpp>

namespace anim
{
class Entity;
class MorphTarget;
class SharedResources;
class Mesh;

class MorphTargetComponent : public ComponentBase<MorphTargetComponent>
{
	friend class SharedResources;

	static const uint32_t max_morph_target = 3;

	struct MorphInfo
	{
		size_t morph_target_index;
		size_t location;
	};

public:
	const std::vector<std::shared_ptr<MorphTarget>>& get_morph_targets()
	{
		return morph_targets_;
	}
	const std::vector<float>& get_weight()
	{
		return weight_;
	}
	void on_changed(size_t a_idx, float a_weight)
	{
		if (morph_targets_.size() < a_idx || weight_.size() < a_idx)
		{
			return;
		}
		a_weight = glm::clamp(a_weight, 0.0f, 1.0f);
		auto& morph_target = morph_targets_[a_idx];
		float& before_weight = weight_[a_idx];
		if (glm::abs(before_weight - a_weight) < glm::epsilon<float>())
		{
			return;
		}
		bool isNeedInit = false;
		auto it = std::find_if(activate_morph_target_indexes_.begin(), activate_morph_target_indexes_.end(),
							   [a_idx](const auto& info) { return info.morph_target_index == a_idx; });
		if (it == activate_morph_target_indexes_.end())
		{
			int location = activate_morph_target_indexes_.size();
			if (activate_morph_target_indexes_.size() >= max_morph_target)
			{
				location = activate_morph_target_indexes_.front().location;
				set_zero(activate_morph_target_indexes_.front());
				activate_morph_target_indexes_.pop_front();
			}
			activate_morph_target_indexes_.emplace_back(a_idx, location);
			isNeedInit = true;
		}
		else
		{
			int location = it->location;
			activate_morph_target_indexes_.erase(it);
			activate_morph_target_indexes_.emplace_back(a_idx, location);
		}
		set_weight(activate_morph_target_indexes_.back(), a_weight, isNeedInit);
	}

protected:
	void set_morph_targets(std::vector<std::shared_ptr<MorphTarget>>&& a_morph_targets)
	{
		morph_targets_ = std::move(a_morph_targets);
		weight_.clear();
		weight_.resize(morph_targets_.size(), 0.0f);
	}
	void set_zero(MorphInfo& info)
	{
		weight_[info.morph_target_index] = 0;
	}
	void set_weight(MorphInfo& info, float weight, bool isNeedInit)
	{
		auto& morph_target = morph_targets_[info.morph_target_index];
		weight_[info.morph_target_index] = weight;
		int count = morph_target->get_count();
		for (int i = 0; i < count; i++)
		{
			const auto* morph_delta = morph_target->get_morph_delta(i);
			auto* mesh = const_cast<Mesh*>(morph_target->get_mesh(i));

			if (isNeedInit)
			{
				mesh->init_morph(info.location, morph_delta);
			}
			mesh->set_morph_weight(info.location, weight);
		}
	}

private:
	std::vector<std::shared_ptr<MorphTarget>> morph_targets_;
	std::vector<float> weight_;
	std::deque<MorphInfo> activate_morph_target_indexes_;
};
}	 // namespace anim

#endif