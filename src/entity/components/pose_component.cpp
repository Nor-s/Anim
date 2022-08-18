#include "pose_component.h"
#include "../../util/log.h"
#include "animation_component.h"
#include "animation/animation.h"

namespace anim
{
    PoseComponent::PoseComponent()
        : bone_info_map_(), shader_(nullptr), animator_(nullptr), animation_component_(nullptr), armature_root_(nullptr), bone_count_(0)
    {
        LOG("PoseComponent");
    }
    void PoseComponent::set_bone_info_map(std::unordered_map<std::string, BoneInfo> &bone_info_map)
    {

        if (bone_info_map_.empty())
        {
            printf("%d , %d ", (int)bone_info_map_.size(), (int)bone_info_map.size());
        }
        else
        {
            printf("%d , %d ", (int)bone_info_map_.size(), (int)bone_info_map.size());
        }
        for (const auto &entry : bone_info_map)
        {
            bone_info_map_[entry.first] = BoneInfo{entry.second.get_id(), entry.second.get_offset()};
        }
        bone_count_ = bone_info_map_.size();
    }
    void PoseComponent::set_animator(Animator *animator)
    {
        animator_ = animator;
    }
    void PoseComponent::set_animation_component(AnimationComponent *animation_component)
    {
        animation_component_ = animation_component;
    }
    void PoseComponent::set_shader(Shader *shader)
    {
        shader_ = shader;
    }
    void PoseComponent::add_bone(const std::string &name, BoneInfo info)
    {
        if (bone_info_map_.find(name) != bone_info_map_.end())
        {
            return;
        }
        bone_info_map_[name] = info;
        bone_count_++;
    }
    void PoseComponent::set_armature_root(Entity *armature_root)
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

    Entity *PoseComponent::get_root_entity()
    {
        return armature_root_;
    }
    Animator *PoseComponent::get_animator()
    {
        return animator_;
    }
    void PoseComponent::add_and_replace_bone(const std::string &name, const glm::mat4 &transform)
    {
        Animation *animation = (animation_component_) ? animation_component_->get_mutable_animation() : nullptr;
        if (animation)
        {
            animation->add_and_replace_bone(name, transform, animator_->get_current_time());
        }
    }

}