#ifndef GLCPP_ANIM_ANIMATION_HPP
#define GLCPP_ANIM_ANIMATION_HPP

#include <glm/glm.hpp>
#include <assimp/scene.h>

#include <functional>
#include <vector>
#include <map>
#include <iostream>

#include "bone.hpp"
#include "../model.h"

namespace glcpp
{

    struct AssimpNodeData
    {
        glm::mat4 transformation;
        std::string name;
        int childrenCount;
        std::vector<AssimpNodeData> children;
    };

    class Animation
    {
    public:
        Animation() = default;

        Animation(const char *animationPath, Model *model)
        {
            Assimp::Importer importer;
            const aiScene *scene = importer.ReadFile(animationPath, aiProcess_Triangulate);

            if (!scene || !scene->mRootNode || !scene->HasAnimations())
            {
                std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            }
            else
            {
                auto animation = scene->mAnimations[0];
                m_Duration = animation->mDuration;
                m_TicksPerSecond = animation->mTicksPerSecond;

                // TODO: what? globalTransformation
                // aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
                // globalTransformation = globalTransformation.Inverse();

                ReadHeirarchyData(m_RootNode, scene->mRootNode);
                // FBX file sometime missing bones.
                ReadMissingBones(animation, *model);
            }
        }

        ~Animation()
        {
        }

        Bone *FindBone(const std::string &name)
        {
            auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
                                     [&](const Bone &Bone)
                                     {
                                         return Bone.GetBoneName() == name;
                                     });
            if (iter == m_Bones.end())
                return nullptr;
            else
                return &(*iter);
        }

        inline float GetTicksPerSecond() { return m_TicksPerSecond; }
        inline float GetDuration() { return m_Duration; }
        inline const AssimpNodeData &GetRootNode() { return m_RootNode; }
        inline const std::map<std::string, BoneInfo> &GetBoneIDMap()
        {
            return m_BoneInfoMap;
        }

    private:
        void ReadMissingBones(const aiAnimation *animation, Model &model)
        {
            int size = animation->mNumChannels;

            auto &boneInfoMap = model.get_mutable_bone_info_map(); // getting m_BoneInfoMap from Model class
            int &boneCount = model.get_mutable_bone_count();       // getting the m_BoneCounter from Model class

            std::cout << "missing Bons\n";

            // reading channels(bones engaged in an animation and their keyframes)
            for (int i = 0; i < size; i++)
            {
                auto channel = animation->mChannels[i];
                std::string boneName = channel->mNodeName.data;
                std::cout << boneName << "\n";

                if (boneInfoMap.find(boneName) == boneInfoMap.end())
                {
                    boneInfoMap[boneName].id = boneCount;
                    boneCount++;
                }
                m_Bones.push_back(Bone(channel->mNodeName.data,
                                       boneInfoMap[channel->mNodeName.data].id, channel));
            }

            m_BoneInfoMap = boneInfoMap;
        }

        void ReadHeirarchyData(AssimpNodeData &dest, const aiNode *src)
        {
            assert(src);

            dest.name = src->mName.data;
            dest.transformation = AiMatToGlmMat(src->mTransformation);
            dest.childrenCount = src->mNumChildren;

            for (unsigned int i = 0; i < src->mNumChildren; i++)
            {
                AssimpNodeData newData;
                ReadHeirarchyData(newData, src->mChildren[i]);
                dest.children.push_back(newData);
            }
        }
        float m_Duration;
        int m_TicksPerSecond;
        std::vector<Bone> m_Bones;
        AssimpNodeData m_RootNode;
        std::map<std::string, BoneInfo> m_BoneInfoMap;
    };

}

#endif