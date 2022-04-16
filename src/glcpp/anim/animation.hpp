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

            if (!scene || !scene->mRootNode)
            {
                std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            }
            else
            {
                auto animation = scene->mAnimations[0];
                m_Duration = animation->mDuration;
                m_TicksPerSecond = animation->mTicksPerSecond;

                // TODO: what? globalTransformation
                // aiMatrix4x4 &globalTransformation = scene->mRootNode->mTransformation;
                // globalTransformation = globalTransformation.Inverse();

                ReadHeirarchyData(m_RootNode, scene->mRootNode);
                // FBX file sometime missing bones.
                ReadMissingBones(animation, *model);
            }
            std::cout << "animation root node count: " << assimpNodeDataCount << "\n";
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
        inline const AssimpNodeData &GetRootNode() { return m_RootNode; }
        inline const std::map<std::string, BoneInfo> &GetBoneIDMap()
        {
            return m_BoneInfoMap;
        }

    private:
        void ReadMissingBones(const aiAnimation *animation, Model &model)
        {
            int size = animation->mNumChannels;
            std::cout << "animation bone size: " << size << "\n";

            auto &boneInfoMap = model.get_mutable_bone_info_map(); // getting m_BoneInfoMap from Model class

            // reading channels(bones engaged in an animation and their keyframes)
            for (int i = 0; i < size; i++)
            {
                auto channel = animation->mChannels[i];

                m_Bones[channel->mNodeName.data] = std::make_unique<Bone>(channel->mNodeName.data,
                                                                          boneInfoMap[channel->mNodeName.data].id, channel);
            }

            m_BoneInfoMap = boneInfoMap;
        }

        void ReadHeirarchyData(AssimpNodeData &dest, const aiNode *src)
        {
            assert(src);
            assimpNodeDataCount++;
            dest.name = src->mName.data;
            dest.transformation = AiMatToGlmMat(src->mTransformation);
            dest.childrenCount = src->mNumChildren;

            std::vector<std::pair<int, int>> sorted_child;
            for (unsigned int i = 0; i < src->mNumChildren; i++)
            {
                int idx = i;
                int num_child = src->mChildren[i]->mNumChildren;
                sorted_child.push_back({num_child, idx});
            }
            std::sort(sorted_child.begin(), sorted_child.end(), [](std::pair<int, int> &a, std::pair<int, int> &b)
                      {
                if(a.first == b.first) {
                    return a.second < b.second;
                }
                return a.first > b.first; });

            for (auto idx : sorted_child)
            {
                AssimpNodeData newData;
                ReadHeirarchyData(newData, src->mChildren[idx.second]);
                dest.children.push_back(newData);
            }
        }
        float m_Duration;
        int m_TicksPerSecond;
        std::map<std::string, std::unique_ptr<Bone>> m_Bones;
        AssimpNodeData m_RootNode;
        int assimpNodeDataCount = 0;
        std::map<std::string, BoneInfo> m_BoneInfoMap;
    };

}

#endif