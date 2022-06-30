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
        if (entity == nullptr || shader == nullptr)
        {
            return;
        }

        AnimationComponent *animation = entity->get_mutable_pointer_animation_component();
        if (!animation)
        {
            return;
        }

        bool &is_stop = animation->get_mutable_is_stop();
        float &current_time = animation->get_mutable_current_time();
        uint32_t duration = animation->get_custom_duration();
        const float fps = animation->get_mutable_fps();
        bool is_loop = animation->get_mutable_is_loop();

        if (!is_stop)
        {
            current_time += fps * dt;
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

    void Animator::calculate_bone_transform(const Model *model, const ModelNode *node, Animation *animation, glm::mat4 parentTransform)
    {
        std::string node_name = node->name;
        // 모델의 바인딩포즈 렌더링
        glm::mat4 node_transform = node->initial_transformation;

        // 애니메이션 렌더링
        Bone *Bone = animation->FindBone(node_name);
        if (Bone != nullptr)
        {
            Bone->Update(current_time_, factor_);
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
            calculate_bone_transform(model, node->childrens[i].get(), animation, globalTransformation);
    }
}