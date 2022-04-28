#ifndef GLCPP_ANIM_ANIMATOR_HPP
#define GLCPP_ANIM_ANIMATOR_HPP

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/glm.hpp>
#include <map>
#include <vector>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>

#include "../model.h"
#include "animation.hpp"
#include "bone.hpp"

namespace glcpp
{
    class Animator
    {
    public:
        Animator()
        {
            current_time_ = 0.0;
            final_bone_matrices_.reserve(128);
            for (int i = 0; i < 128; i++)
                final_bone_matrices_.push_back(glm::mat4(1.0f));
        }

        void update_animation(float dt, const Model *model)
        {
            delta_time_ = dt;
            if (animations_.size() > current_animation_idx_ && animations_[current_animation_idx_])
            {
                current_time_ += animations_[current_animation_idx_]->GetTicksPerSecond() * dt;
                current_time_ = fmod(current_time_, animations_[current_animation_idx_]->GetDuration());
                calculate_bone_transform(model, model->get_root_node(), glm::mat4(1.0f));
            }
        }

        void add_animation(const char *animation_path)
        {
            animations_.push_back(std::make_shared<Animation>(animation_path));
            play_animation(animations_.size() - 1);
            // play_animation(0);
        }
        std::shared_ptr<Animation> &get_mutable_current_animation()
        {
            return animations_[current_animation_idx_];
        }

        // TODO: How to implement retargeting (anim_binding * trs => anim_binding-1 * anim_binding * trs => trs, model_binding*trs)
        void play_animation(uint32_t animation_id)
        {
            if (animation_id < animations_.size())
                current_animation_idx_ = animation_id;
            current_time_ = 0.0f;
        }

        void calculate_bone_transform(const Model *model, const ModelNode *node, glm::mat4 parentTransform)
        {
            std::string node_name = node->name;
            // 모델의 바인딩포즈 렌더링
            glm::mat4 node_transform = node->initial_transformation;

            // 애니메이션 렌더링
            Bone *Bone = animations_[current_animation_idx_]->FindBone(node_name);
            // retargeting mixamo
            auto &inverse_map = animations_[current_animation_idx_]->get_mutable_inverse_transform();
            if (Bone != nullptr && !is_stop_)
            {
                Bone->Update(current_time_);
                node_transform *= inverse_map[node_name] * Bone->GetLocalTransform();
            }
            // 모델 자체를 렌더링.
            if (is_stop_)
            {
                node_transform = node->get_mix_transformation();
            }
            // FK
            glm::mat4 globalTransformation = parentTransform * node_transform;

            auto &bone_info_map = model->get_bone_info_map();
            auto it = bone_info_map.find(node_name);
            if (it != bone_info_map.end())
            {
                // 역바인딩변환 행렬과 변환행렬을 곱해줌 (본공간 => 로컬공간)
                final_bone_matrices_[it->second.get_id()] = globalTransformation * it->second.get_offset();
            }

            for (int i = 0; i < node->childrens.size(); i++)
                calculate_bone_transform(model, node->childrens[i].get(), globalTransformation);
        }

        const std::vector<glm::mat4> &get_final_bone_matrices() const
        {
            return final_bone_matrices_;
        }
        void set_is_stop(bool is_stop)
        {
            is_stop_ = is_stop;
        }

    private:
        std::vector<glm::mat4> final_bone_matrices_;
        std::vector<std::shared_ptr<Animation>> animations_;
        std::map<std::string, std::shared_ptr<Animation>> animation_map;
        uint32_t current_animation_idx_ = -1;
        float current_time_;
        float delta_time_;
        bool is_stop_ = false;
    };
}

#endif