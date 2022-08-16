#ifndef ANIM_ENTITY_COMPONENT_POSE_COMPONENT_H
#define ANIM_ENTITY_COMPONENT_POSE_COMPONENT_H

#include "component.h"
#include "../../resources/model.h"
#include "../../../animation/animator.h"
#include "animation_component.h"
#include "../../graphics/shader.h"

namespace anim
{
    class PoseComponent : public ComponentBase<PoseComponent>
    {
    public:
        PoseComponent() = default;
        void set_bone_info_map(std::unordered_map<std::string, BoneInfo> &bone_info_map)
        {
            for (const auto &entry : bone_info_map)
            {
                bone_info_map_[entry.first] = BoneInfo{entry.second.get_id(), entry.second.get_offset()};
            }
        }
        void set_animator(Animator *animator)
        {
            animator_ = animator;
        }
        void set_animation_component(AnimationComponent *animation_component)
        {
            animation_component_ = animation_component;
        }
        void set_shader(Shader *shader)
        {
            shader_ = shader;
        }
        void add_bone(const std::string &name, BoneInfo info)
        {
            if (bone_info_map_.find(name) != bone_info_map_.end())
            {
                return;
            }
            bone_info_map_[name] = info;
            bone_count_++;
        }
        void set_armature_root(Entity *armature_root)
        {
            armature_root_ = armature_root;
        }
        void set_dt(float *dt)
        {
            dt_ = dt;
        }
        void update() override
        {
            animator_->update_animation(*dt_, animation_component_, armature_root_, shader_);
        }

    private:
        std::unordered_map<std::string, BoneInfo> bone_info_map_;
        Shader *shader_{nullptr};
        Animator *animator_{nullptr};
        AnimationComponent *animation_component_{nullptr};
        Entity *armature_root_{nullptr};
        float *dt_{nullptr};
        int bone_count_ = 0;
    };

}

#endif