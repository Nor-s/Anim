#ifndef GLCPP_ANIM_ANIMATION_HPP
#define GLCPP_ANIM_ANIMATION_HPP

#include <glm/glm.hpp>
#include <assimp/scene.h>

#include <functional>
#include <vector>
#include <map>
#include <iostream>
#include <string>

#include "bone.hpp"
#include "../utility.hpp"
namespace glcpp
{
    class Animation
    {
    public:
        Animation() = default;

        Animation(const char *animationPath)
        {
            Assimp::Importer importer;
            unsigned int assimp_read_flag =
                aiProcess_SortByPType |
                aiProcess_ValidateDataStructure |
                aiProcess_LimitBoneWeights;

            importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
            const aiScene *scene = importer.ReadFile(animationPath, assimp_read_flag);

            if (!scene || !scene->mRootNode || !scene->HasAnimations())
            {
                std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << "\n";
            }
            else
            {
                auto animation = scene->mAnimations[0];
                m_Duration = animation->mDuration;
                m_TicksPerSecond = animation->mTicksPerSecond;
                std::cout << m_Duration << " " << m_TicksPerSecond << "\n";
                process_bones(animation, scene->mRootNode);
            }
        }

        ~Animation()
        {
        }

        Bone *FindBone(const std::string &name)
        {
            auto iter = m_Bones.find(name);
            if (iter == m_Bones.end())
                return nullptr;
            else
                return &(*(iter->second));
        }
        std::map<std::string, glm::mat4> &get_mutable_bone_transform()
        {
            return animation_init_transform_map_;
        }

        inline float GetTicksPerSecond() { return m_TicksPerSecond; }
        inline float GetDuration() { return m_Duration; }

    private:
        void process_bones(const aiAnimation *animation, const aiNode *root_node)
        {
            int size = animation->mNumChannels;

            // reading channels(bones engaged in an animation and their keyframes)
            for (int i = 0; i < size; i++)
            {
                auto channel = animation->mChannels[i];
                std::string bone_name = channel->mNodeName.C_Str();
                bone_name = bone_name.substr(bone_name.find_last_of(':') + 1);

                m_Bones[bone_name] = std::make_unique<Bone>(bone_name, channel);
                const aiNode *node = root_node->FindNode(channel->mNodeName);
                if (node)
                {
                    animation_init_transform_map_[bone_name] = AiMatToGlmMat(node->mTransformation);
                }
            }
        }

        float m_Duration;
        int m_TicksPerSecond;
        std::map<std::string, glm::mat4> animation_init_transform_map_;
        std::map<std::string, std::unique_ptr<Bone>> m_Bones;

        // timestamp를 좌표압축 => 리넘버링해야함. 이는 모든 뼈의 정보를 취합한것
        std::vector<uint32_t> scale_keys_;
        std::vector<uint32_t> position_keys_;
        std::vector<uint32_t> rotation_keys_;

        // 특정한 키의 뼈들, 그리고 그 뼈의 키 변환을 강조해야함
        // id -> bones ..... bones key t [id], s[id], r[id]
        std::map<uint32_t, std::vector<Bone *>> bones_key_map;
        // 키 => 뼈
    };

}

#endif