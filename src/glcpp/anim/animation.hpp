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
#include "../model.h"

namespace glcpp
{
    class Animation
    {
    public:
        Animation() = default;

        Animation(const char *animationPath, Model *model)
        {
            Assimp::Importer importer;
            unsigned int assimp_read_flag = 
                                            aiProcess_SortByPType |
                                            aiProcess_ValidateDataStructure ;
            // assimp_read_flag |= aiProcess_JoinIdenticalVertices;
            // assimp_read_flag |= aiProcess_FlipWindingOrder;
            assimp_read_flag |= aiProcess_LimitBoneWeights;
            // assimp_read_flag |= aiProcess_FindInvalidData;
            importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
            const aiScene *scene = importer.ReadFile(animationPath, assimp_read_flag);

            if (!scene || !scene->mRootNode)
            {
                std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << "\n";
            }
            else
            {
                auto animation = scene->mAnimations[0];
                m_Duration = animation->mDuration;
                m_TicksPerSecond = animation->mTicksPerSecond;

                process_bones(animation, *model);
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

        inline float GetTicksPerSecond() { return m_TicksPerSecond; }
        inline float GetDuration() { return m_Duration; }

    private:
        void process_bones(const aiAnimation *animation, Model &model)
        {
            int size = animation->mNumChannels;

            // reading channels(bones engaged in an animation and their keyframes)
            for (int i = 0; i < size; i++)
            {
                auto channel = animation->mChannels[i];
                std::string bone_name = channel->mNodeName.C_Str();
                bone_name = bone_name.substr(bone_name.find_last_of(':') + 1);

                m_Bones[bone_name] = std::make_unique<Bone>(bone_name, channel);
            }
        }

        float m_Duration;
        int m_TicksPerSecond;
        std::map<std::string, std::unique_ptr<Bone>> m_Bones;
    };

}

#endif