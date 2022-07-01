#include "animator.h"

#include <glm/gtx/matrix_decompose.hpp>
#include "entity.h"
#include "model.h"
#include "anim/bone.hpp"
#include "anim/animation.hpp"
#include "utility.hpp"
#include "shader.h"
#include "component/animation_component.h"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>

namespace glcpp
{

    Animator::Animator()
    {
        final_bone_matrices_.reserve(MAX_BONE_NUM);
        for (unsigned int i = 0U; i < MAX_BONE_NUM; i++)
            final_bone_matrices_.push_back(glm::mat4(1.0f));
    }

    void Animator::update_animation(float dt, Entity *entity, Shader *shader)
    {
        assert(entity && shader);
        AnimationComponent *animation = entity->get_mutable_pointer_animation_component();
        assert(animation);

        bool &is_stop = animation->get_mutable_is_stop();
        float &current_time = animation->get_mutable_current_time();
        uint32_t duration = animation->get_custom_duration();
        const float fps = animation->get_mutable_fps();
        bool is_loop = animation->get_mutable_is_loop();

        if (!is_stop)
        {
            if (dt <= 0)
            {
                current_time = 0.0f;
            }
            else
            {
                current_time += fps * dt;
            }
            if (current_time > duration && !is_loop)
            {
                current_time = duration - 0.01f;
                is_stop = true;
            }
            else
            {
                current_time = fmod(current_time, duration);
            }
        }
        current_time_ = current_time;
        factor_ = animation->get_ticks_per_second_factor();
        auto model = entity->get_mutable_model();
        calculate_bone_transform(model, model->get_root_node(), animation->get_mutable_animation(), glm::mat4(1.0f));
        shader->use();
        for (int i = 0; i < MAX_BONE_NUM; ++i)
        {
            shader->set_mat4("finalBonesMatrices[" + std::to_string(i) + "]", final_bone_matrices_[i]);
        }
    }

    void Animator::calculate_bone_transform(const Model *model, const ModelNode *node, Animation *animation, const glm::mat4 &parentTransform)
    {
        const std::string &node_name = node->name;
        // 모델의 바인딩포즈 렌더링
        glm::mat4 global_transformation = parentTransform * node->initial_transformation;

        // 애니메이션
        auto bone = animation->find_bone(node_name);
        if (bone != nullptr)
        {
            global_transformation *= bone->get_local_transform(current_time_, factor_);
        }
        // FK

        auto bone_info = model->get_pointer_bone_info(node_name);
        if (bone_info)
        {
            // 역바인딩변환 행렬과 변환행렬을 곱해줌 (본공간 => 로컬공간)
            final_bone_matrices_[bone_info->get_id()] = global_transformation * bone_info->get_offset();
        }

        for (size_t i = 0; i < node->childrens.size(); i++)
            calculate_bone_transform(model, node->childrens[i].get(), animation, global_transformation);
    }
}