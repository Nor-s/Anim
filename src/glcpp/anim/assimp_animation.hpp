#ifndef GLCPP_ANIM_ASSIMP_ANIMATION_HPP
#define GLCPP_ANIM_ASSIMP_ANIMATION_HPP

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
#include "animation.hpp"
namespace glcpp
{
    class AssimpAnimation : public Animation
    {
    public:
        AssimpAnimation() = default;

        AssimpAnimation(const char *animation_path)
        {
            type = AnimationType::Assimp;
            Assimp::Importer importer;
            unsigned int assimp_read_flag =
                aiProcess_SortByPType |
                aiProcess_ValidateDataStructure |
                aiProcess_LimitBoneWeights;

            importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
            const aiScene *scene = importer.ReadFile(animation_path, assimp_read_flag);

            if (!scene || !scene->mRootNode || !scene->HasAnimations())
            {
                std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << "\n";
            }
            else
            {
                auto animation = scene->mAnimations[0];
                name_ = animation_path;
                duration_ = animation->mDuration;
                ticks_per_second_ = animation->mTicksPerSecond;
                process_bones(animation, scene->mRootNode);
            }
        }

        virtual ~AssimpAnimation()
        {
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
                    bone_inverse_transform_map_[bone_name] = glm::inverse(AiMatToGlmMat(node->mTransformation));
                    name_bone_map_[bone_name] = std::make_unique<Bone>(bone_name, channel, bone_inverse_transform_map_[bone_name]);
                }
            }
        }

    private:
        std::map<std::string, glm::mat4> bone_inverse_transform_map_;
    };

}

#endif