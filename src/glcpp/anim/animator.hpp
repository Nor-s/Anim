#ifndef GLCPP_ANIM_ANIMATOR_HPP
#define GLCPP_ANIM_ANIMATOR_HPP

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/glm.hpp>
#include <map>
#include <vector>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>

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

        void UpdateAnimation(float dt)
        {
            m_DeltaTime = dt;
            if (m_CurrentAnimation)
            {
                m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
                m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
                CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f), 0);
            }
        }

        void PlayAnimation(Animation *pAnimation)
        {
            m_CurrentAnimation = pAnimation;
            m_CurrentTime = 0.0f;
        }

        void CalculateBoneTransform(const AssimpNodeData *node, glm::mat4 parentTransform, int depth)
        {
            std::string nodeName = node->name;
            glm::mat4 nodeTransform = node->transformation;

            Bone *Bone = m_CurrentAnimation->FindBone(nodeName);
            if (Bone != nullptr && !is_stop_)
            {
                Bone->Update(m_CurrentTime);
                nodeTransform = Bone->GetLocalTransform();
            }
            if (is_stop_)
            {
                glm::vec3 scale;
                glm::quat rotation;
                glm::vec3 translation;
                glm::vec3 skew;
                glm::vec4 perspective;

                glm::decompose(nodeTransform, scale, rotation, translation, skew, perspective);

                // std::cout << nodeName << "\n";
                // std::cout << "scale: " << scale.x << ", " << scale.y << ", " << scale.z << "\n";
                // std::cout << "translation: " << translation.x << ", " << translation.y << ", " << translation.z << "\n";
                // std::cout << "rotation: " << rotation.x << ", " << rotation.y << ", " << rotation.z << ", " << rotation.w << "\n";
                translation += node->translation;
                scale = node->scale;
                auto tt = glm::translate(glm::mat4(1.0f), translation);
                auto ss = glm::scale(glm::mat4(1.0f), scale);
                auto rx = glm::rotate(glm::mat4_cast(rotation), node->rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
                auto ry = glm::rotate(rx, node->rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
                auto rr = glm::rotate(ry, node->rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

                nodeTransform = tt * rr * ss;
            }
            glm::mat4 globalTransformation = parentTransform * nodeTransform;

            auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
            if (boneInfoMap.find(nodeName) != boneInfoMap.end())
            {
                int index = boneInfoMap[nodeName].id;
                glm::mat4 offset = boneInfoMap[nodeName].offset;
                m_FinalBoneMatrices[index] = globalTransformation * offset;
            }

            for (int i = 0; i < node->childrenCount; i++)
                CalculateBoneTransform(&node->children[i], globalTransformation, depth + 1);
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