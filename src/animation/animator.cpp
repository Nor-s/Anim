#include "animator.h"

#include <glm/gtx/matrix_decompose.hpp>
#include "entity.h"
#include "bone.h"
#include "../util/utility.h"
#include "shader.h"
#include "../entity/components/animation_component.h"
#include "../entity/components/renderable/armature_component.h"
#include "animation.h"
#include "json_animation.h"
#include "assimp_animation.h"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>

namespace anim
{

    Animator::Animator()
    {
        final_bone_matrices_.reserve(MAX_BONE_NUM);
        for (unsigned int i = 0U; i < MAX_BONE_NUM; i++)
            final_bone_matrices_.push_back(glm::mat4(1.0f));
    }

    void Animator::update_animation(float dt, AnimationComponent *animation, Entity *root, Shader *shader)
    {
        assert(animation);

        bool &is_stop = animation->get_mutable_is_stop();
        float &current_time = animation->get_mutable_current_time();
        uint32_t duration = animation->get_custom_duration();
        const float fps = animation->get_mutable_fps();
        bool is_loop = animation->get_mutable_is_loop();

        if (!is_stop)
        {
            float time = current_time + fps * dt;
            if (dt <= 0)
            {
                time = 0.0f;
            }
            current_time = fmod(time, duration);
            if (time > duration)
            {
                is_stop = !is_loop;
                current_time = 0.0f;
            }
        }
        current_time_ = current_time;
        factor_ = animation->get_ticks_per_second_factor();
        calculate_bone_transform(root, animation->get_mutable_animation(), glm::mat4(1.0f));
        shader->use();
        for (int i = 0; i < MAX_BONE_NUM; ++i)
        {
            shader->set_mat4("finalBonesMatrices[" + std::to_string(i) + "]", final_bone_matrices_[i]);
        }
    }

    void Animator::calculate_bone_transform(Entity *entity, Animation *animation, const glm::mat4 &parentTransform)
    {
        const std::string &node_name = entity->get_name();
        auto armature = entity->get_component<ArmatureComponent>();
        glm::mat4 global_transformation = parentTransform;

        // 바인딩 포즈
        global_transformation *= entity->get_local_transformation();

        // 애니메이션
        auto bone = animation->find_bone(node_name);
        if (bone != nullptr)
        {
            // 애니메이션 포즈
            global_transformation *= bone->get_local_transform(current_time_, factor_);
        }
        // FK
        int id = armature->get_id();
        auto &offset = armature->get_bone_offset();
        if (id < MAX_BONE_NUM)
        {
            // 역바인딩변환 행렬과 변환행렬을 곱해줌 (본공간 => 로컬공간)
            final_bone_matrices_[id] = global_transformation * offset;
        }

        auto &children = entity->get_mutable_children();
        size_t size = children.size();
        entity->set_world_transformation(global_transformation);

        for (size_t i = 0; i < size; i++)
        {
            calculate_bone_transform(children[i].get(), animation, global_transformation);
        }
    }
}