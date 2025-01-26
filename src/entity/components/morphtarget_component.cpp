#include "morphtarget_component.h"

#include "../entity.h"

#include "animation/animation.h"
#include "animation_component.h"
#include "../animation/morph_target.h"
#include "animation/animator.h"
#include "shader.h"

#include "../graphics/mesh.h"

#include <algorithm>

namespace anim
{

void MorphTargetComponent::on_changed(size_t a_idx, float a_weight)
{
	if (morph_targets_.size() < a_idx || weights_.size() < a_idx)
	{
		return;
	}
	a_weight = glm::clamp(a_weight, 0.0f, 1.0f);
	auto& morph_target = morph_targets_[a_idx];
	float& before_weight = weights_[a_idx];
	if (glm::abs(before_weight - a_weight) < 0.001f && is_active_[a_idx])
	{
		return;
	}
	bool isNeedInit = false;
	auto it = std::find_if(active_morph_target_indexes_.begin(), active_morph_target_indexes_.end(),
						   [a_idx](const auto& info) { return info.morph_target_index == a_idx; });
	if (it == active_morph_target_indexes_.end())
	{
		int location = active_morph_target_indexes_.size();
		if (active_morph_target_indexes_.size() >= Animator::GetMaxMorph())
		{
			location = active_morph_target_indexes_.front().location;
			int idx = active_morph_target_indexes_.front().morph_target_index;
			//set_zero(active_morph_target_indexes_.front());
			active_morph_target_indexes_.pop_front();
			is_active_[idx] = false;
		}
		active_morph_target_indexes_.emplace_back(a_idx, location);
		it = std::next(active_morph_target_indexes_.end(), -1);
		isNeedInit = true;
	}
	else
	{
		MorphInfo temp = *it;
		active_morph_target_indexes_.erase(it);
		active_morph_target_indexes_.push_back(temp);
	}

	set_weight(active_morph_target_indexes_.back(), a_weight, isNeedInit);
}
void MorphTargetComponent::update_morph_datas(MorphDatas& datas, size_t count)
{
	size_t size = std::min(datas.size(), count);
	std::deque<std::pair<int, float>> temp;
	for (int i = 0; i < size; i++)
	{
		auto& data = datas[i];
		float weight = data.first;
		size_t index = data.second;
		auto it = std::find_if(
			active_morph_target_indexes_.begin(), 
			active_morph_target_indexes_.end(), 
			[index](const auto& info) { return info.morph_target_index == index;
			}
			);
		if (it == active_morph_target_indexes_.end())
		{
			temp.emplace_back(index, weight);
		}
		else
		{
			temp.emplace_front(index, weight);
		}
	}
	for (auto data : temp)
	{
		on_changed(data.first, data.second);
	}
	for (int i = size; i < datas.size(); i++)
	{
		auto& [weight, index] = datas[i];
		weights_[index] = weight;
	}
}
void MorphTargetComponent::set_shader(Shader& shader)
{
	for (int i = 0; i < active_morph_target_indexes_.size(); i++)
	{
		int location = active_morph_target_indexes_[i].location;
		int idx = active_morph_target_indexes_[i].morph_target_index;
		shader.set_float("target_weights[" + std::to_string(location) + "]", weights_[idx]);
	}
}
bool MorphTargetComponent::is_active(int index)
{
	return is_active_[index];
}
const std::string& MorphTargetComponent::get_name(int index)
{
	return morph_targets_[index]->get_name();
}
void MorphTargetComponent::set_zero(MorphInfo& info)
{
	weights_[info.morph_target_index] = 0.0;
}
void MorphTargetComponent::set_weight(MorphInfo& info, float weight, bool isNeedInit)
{
	auto& morph_target = morph_targets_[info.morph_target_index];
	weights_[info.morph_target_index] = weight;
	is_active_[info.morph_target_index] = true;
	int count = morph_target->get_count();
	for (int i = 0; i < count; i++)
	{
		const auto* morph_delta = morph_target->get_morph_delta(i);
		auto* mesh = const_cast<Mesh*>(morph_target->get_mesh(i));

		if (isNeedInit)
		{
			mesh->init_morph(info.location, morph_delta);
		}
	}
}

}	 // namespace anim