#ifndef GLCPP_ANIM_ANIMATION_HPP
#define GLCPP_ANIM_ANIMATION_HPP

#include <glm/glm.hpp>
#include <assimp/scene.h>

#include <functional>
#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <set>

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
                name_ = animationPath;
                duration_ = animation->mDuration;
                ticks_per_second_ = animation->mTicksPerSecond;
                process_bones(animation, scene->mRootNode);
            }
        }

        ~Animation()
        {
        }

        Bone *FindBone(const std::string &name)
        {
            auto iter = name_bone_map_.find(name);
            if (iter == name_bone_map_.end())
                return nullptr;
            else
                return &(*(iter->second));
        }
        std::map<std::string, glm::mat4> &get_mutable_inverse_transform()
        {
            return animation_inverse_transform_map_;
        }

        inline float get_ticks_per_second() { return ticks_per_second_; }
        inline float get_duration() { return duration_; }

        const char *get_name() const
        {
            return name_.c_str();
        }
        const std::map<std::string, std::unique_ptr<Bone>> &get_name_bone_map() const
        {
            return name_bone_map_;
        }
        std::map<std::string, std::unique_ptr<Bone>> &get_mutable_name_bone_map()
        {
            return name_bone_map_;
        }

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

                const aiNode *node = root_node->FindNode(channel->mNodeName);
                if (node)
                {
                    animation_inverse_transform_map_[bone_name] = glm::inverse(AiMatToGlmMat(node->mTransformation));
                    name_bone_map_[bone_name] = std::make_unique<Bone>(bone_name, channel, animation_inverse_transform_map_[bone_name]);
                }
            }
        }

        float duration_;
        int ticks_per_second_;
        std::string name_;
        std::map<std::string, glm::mat4> animation_inverse_transform_map_;
        std::map<std::string, std::unique_ptr<Bone>> name_bone_map_;
    };

}

#endif