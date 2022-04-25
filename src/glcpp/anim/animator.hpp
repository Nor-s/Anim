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
        Animator(Animation *animation)
        {
            m_CurrentTime = 0.0;
            m_CurrentAnimation = animation;

            m_FinalBoneMatrices.reserve(128);

            for (int i = 0; i < 128; i++)
                m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
        }

        void UpdateAnimation(float dt, const ModelNode *node)
        {
            m_DeltaTime = dt;
            if (m_CurrentAnimation)
            {
                m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
                m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
                CalculateBoneTransform(node, glm::mat4(1.0f), 0);
            }
        }

        void PlayAnimation(Animation *pAnimation)
        {
            m_CurrentAnimation = pAnimation;
            m_CurrentTime = 0.0f;
        }

        void CalculateBoneTransform(const ModelNode *node, glm::mat4 parentTransform, int depth)
        {
            std::string node_name = node->name;
            glm::mat4 node_transform = node->initial_transformation;

            Bone *Bone = m_CurrentAnimation->FindBone(node_name);
            if (Bone != nullptr && !is_stop_)
            {
                Bone->Update(m_CurrentTime);
                node_transform = Bone->GetLocalTransform();
            }
            if (is_stop_)
            {
                node_transform = node->get_mix_transformation();
            }
            glm::mat4 globalTransformation = parentTransform * node_transform;

            auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
            if (boneInfoMap.find(node_name) != boneInfoMap.end())
            {
                int index = boneInfoMap[node_name].id;
                glm::mat4 offset = boneInfoMap[node_name].offset;
                m_FinalBoneMatrices[index] = globalTransformation * offset;
            }

            for (int i = 0; i < node->childrens.size(); i++)
                CalculateBoneTransform(node->childrens[i].get(), globalTransformation, depth + 1);
        }

        std::vector<glm::mat4> GetFinalBoneMatrices()
        {
            return m_FinalBoneMatrices;
        }
        void set_is_stop(bool is_stop)
        {
            is_stop_ = is_stop;
        }

    private:
        std::vector<glm::mat4> m_FinalBoneMatrices;
        Animation *m_CurrentAnimation;
        float m_CurrentTime;
        float m_DeltaTime;
        bool is_stop_ = false;
    };
}

#endif