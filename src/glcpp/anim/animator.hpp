#ifndef GLCPP_ANIM_ANIMATOR_HPP
#define GLCPP_ANIM_ANIMATOR_HPP

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/glm.hpp>
#include <map>
#include <filesystem>
#include <vector>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>

#include "../model.h"
#include "animation.hpp"
#include "assimp_animation.hpp"
#include "json_animation.hpp"
#include "bone.hpp"
#include "utility.hpp"

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
            if (animations_.size() > current_animation_idx_ && animations_[current_animation_idx_])
            {
                if (!is_stop_)
                {
                    current_time_ += get_mutable_custom_tick_per_second() * dt;
                    current_time_ = fmod(current_time_, get_custom_duration());
                }
                calculate_bone_transform(model, model->get_root_node(), glm::mat4(1.0f));
            }
        }

        void add_animation(const char *animation_path)
        {
            std::filesystem::path anim_path(ConvertStringToWString(std::string(animation_path)).c_str());
            for (size_t i = 0; i < animations_.size(); i++)
            {
                if (std::strcmp(animations_[i]->get_name(), animation_path) == 0)
                {
                    return;
                }
            }
            if (anim_path.extension() == ".json")
            {
                animations_.push_back(std::make_shared<JsonAnimation>(animation_path));
            }
            else
            {

                animations_.push_back(std::make_shared<AssimpAnimation>(animation_path));
            }

            if (animations_.back()->get_duration() > 0)
            {
                play_animation(animations_.size() - 1);
            }
            else
            {
                animations_.back().reset();
                animations_.pop_back();
            }
            // play_animation(0);
        }
        std::shared_ptr<Animation> &get_mutable_current_animation()
        {
            return animations_[current_animation_idx_];
        }

        // TODO: How to implement retargeting (anim_binding * trs => anim_binding-1 * anim_binding * trs => trs, model_binding*trs)
        void play_animation(uint32_t animation_id)
        {
            if (animation_id == current_animation_idx_)
            {
                return;
            }
            if (animation_id < animations_.size())
            {
                current_animation_idx_ = animation_id;
                if (animations_[current_animation_idx_]->get_ticks_per_second() >= 120)
                {
                    custom_tick_per_second_ = 24.0f;
                }
                else
                {
                    custom_tick_per_second_ = animations_[current_animation_idx_]->get_ticks_per_second();
                }
            }
            current_time_ = 0.0f;
        }

        void calculate_bone_transform(const Model *model, const ModelNode *node, glm::mat4 parentTransform)
        {
            std::string node_name = node->name;
            // 모델의 바인딩포즈 렌더링
            glm::mat4 node_transform = node->initial_transformation;

            // 애니메이션 렌더링
            Bone *Bone = animations_[current_animation_idx_]->FindBone(node_name);
            if (Bone != nullptr)
            {
                Bone->Update(current_time_, (custom_tick_per_second_ / animations_[current_animation_idx_]->get_ticks_per_second()));
                node_transform *= Bone->GetLocalTransform();
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

            for (size_t i = 0; i < node->childrens.size(); i++)
                calculate_bone_transform(model, node->childrens[i].get(), globalTransformation);
        }

        const std::vector<glm::mat4> &get_final_bone_matrices() const
        {
            return final_bone_matrices_;
        }
        bool &get_mutable_is_stop()
        {
            return is_stop_;
        }

        float &get_mutable_custom_tick_per_second()
        {
            return custom_tick_per_second_;
        }
        const uint32_t get_custom_duration() const
        {
            auto duration = animations_[current_animation_idx_]->get_duration();
            auto origin_tick_per_second = animations_[current_animation_idx_]->get_ticks_per_second();
            return static_cast<uint32_t>(duration * (custom_tick_per_second_ / origin_tick_per_second));
        }
        void set_custom_tick_per_second(float tick_per_second)
        {
            if (tick_per_second > 0.0f)
                custom_tick_per_second_ = tick_per_second;
        }

        const uint32_t get_current_frame_num() const
        {
            return static_cast<uint32_t>(roundf(current_time_));
        }

        void set_current_frame_num_to_time(uint32_t frame)
        {
            current_time_ = static_cast<float>(frame);
        }
        float get_origin_current_time(float time)
        {
            auto origin_tick_per_second = animations_[current_animation_idx_]->get_ticks_per_second();
            return time * origin_tick_per_second / get_mutable_custom_tick_per_second();
        }
        const std::vector<const char *> get_animation_name_list() const
        {
            std::vector<const char *> ret;
            for (auto &anim : animations_)
            {
                ret.push_back(anim->get_name());
            }
            return ret;
        }

    private:
        std::vector<glm::mat4> final_bone_matrices_;
        std::vector<std::shared_ptr<Animation>> animations_;
        uint32_t current_animation_idx_ = -1;
        float current_time_;
        bool is_stop_ = false;
        float custom_tick_per_second_ = 24.0f;
    };
}

#endif