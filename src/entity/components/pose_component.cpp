#include "pose_component.h"
#include "pose_component.h"
#include "../../util/log.h"
#include "animation_component.h"
#include "animation/animation.h"
#include "renderable/armature_component.h"
#include "../entity.h"

namespace anim
{
/*
모프타겟 수 제한

렌더링
- 활성화된 웨이트
    - 애니메이터에서 계산된 값 -> 모프타겟 컴포넌트에 업데이트


- 포즈 컴포넌트는 애니메이터, 애니메이션에 접근하여 적절하게 애님이션 데이터를 편집가능
- 애니메이션 컴포넌트는 애니메이션 정보를 가지고 있음, 프레임별로
- 애니메이터는 애니메이션 컴포넌트에 설정된 애니메이션을 기반으로 현재 적용해야할 본의 변환 을 적용함


*/
PoseComponent::PoseComponent()
	: bone_info_map_(), shader_(nullptr), animator_(nullptr), animation_component_(nullptr), armature_root_(nullptr)
{
	LOG("PoseComponent");
}
void PoseComponent::set_bone_info_map(std::unordered_map<std::string, BoneInfo>& bone_info_map)
{
	for (const auto& entry : bone_info_map)
	{
		bone_info_map_[entry.first] = BoneInfo{entry.second.get_id(), entry.second.get_offset()};
	}
}
void PoseComponent::set_animator(Animator* animator)
{
	animator_ = animator;
}
void PoseComponent::set_animation_component(AnimationComponent* animation_component)
{
	animation_component_ = animation_component;
}
void PoseComponent::set_shader(Shader* shader)
{
	shader_ = shader;
}
void PoseComponent::add_bone(const std::string& name, BoneInfo info)
{
	if (bone_info_map_.find(name) != bone_info_map_.end())
	{
		return;
	}
	bone_info_map_[name] = info;
}
void PoseComponent::set_armature_root(Entity* armature_root)
{
	armature_root_ = armature_root;
}
void PoseComponent::update()
{
	if (animation_component_->get_animation())
	{
		animator_->update_animation(animation_component_, armature_root_, shader_);
	}
}

Entity* PoseComponent::get_root_entity()
{
	return armature_root_;
}
Animator* PoseComponent::get_animator()
{
	return animator_;
}
AnimationComponent* PoseComponent::get_animation_component()
{
	return animation_component_;
}

void PoseComponent::insert_or_update_bone(const std::string& name, const glm::mat4& transform)
{
	Animation* animation = (animation_component_) ? animation_component_->get_mutable_animation() : nullptr;
	if (animation)
	{
		animation->insert_or_update_keyframe(name, transform, animator_->get_current_time());
	}
}

void PoseComponent::insert_or_update_morph(int index, const float weight)
{
	Animation* animation = (animation_component_) ? animation_component_->get_mutable_animation() : nullptr;
	if (animation)
	{
		animation->insert_or_update_morph(index, weight, animator_->get_current_time());
	}
}

void PoseComponent::sub_current_bone(const std::string& name)
{
	Animation* animation = (animation_component_) ? animation_component_->get_mutable_animation() : nullptr;
	if (animation)
	{
		animation->remove_keyframe(name, animator_->get_current_time());
	}
}

Entity* PoseComponent::find(int bone_id)
{
	return find(bone_id, armature_root_);
}
Entity* PoseComponent::find(int bone_id, Entity* entity)
{
	auto armature = entity->get_component<ArmatureComponent>();
	if (armature)
	{
		int id = armature->get_id();
		if (bone_id == id)
		{
			return entity;
		}
		auto& child = entity->get_mutable_children();
		for (int i = 0; i < child.size(); i++)
		{
			auto ret = find(bone_id, child[i].get());
			if (ret)
			{
				return ret;
			}
		}
	}
	return nullptr;
}
}	 // namespace anim