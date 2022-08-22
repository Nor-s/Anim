#include "assimp_animation.h"
#include "../util/utility.h"

namespace fs = std::filesystem;

namespace anim
{
    AssimpAnimation::AssimpAnimation(const aiAnimation *animation, const aiScene *scene, const char *path)
    {
        init_animation(animation, scene, path);
    }

    AssimpAnimation::~AssimpAnimation()
    {
    }

    void AssimpAnimation::init_animation(const aiAnimation *animation, const aiScene *scene, const char *path)
    {
        type_ = AnimationType::Assimp;

        path_ = std::string(path);
        fs::path anim_path = fs::u8path(path_);
        name_ = anim_path.filename().string();
        duration_ = animation->mDuration;
        fps_ = animation->mTicksPerSecond;
        process_bones(animation, scene->mRootNode);
    }
    void AssimpAnimation::process_bones(const aiAnimation *animation, const aiNode *root_node)
    {
        int size = animation->mNumChannels;

        // reading channels(bones engaged in an animation and their keyframes)
        for (int i = 0; i < size; i++)
        {
            auto channel = animation->mChannels[i];
            std::string bone_name = channel->mNodeName.C_Str();
            // bone_name = bone_name.substr(bone_name.find_last_of(':') + 1);
            // auto find_mixamorig = bone_name.find("mixamorig");
            // if (find_mixamorig != std::string::npos)
            // {
            //     bone_name = bone_name.substr(find_mixamorig + 9);
            // }
            const aiNode *node = root_node->FindNode(channel->mNodeName);
            if (node)
            {
                name_bone_map_[bone_name] = std::make_unique<Bone>(bone_name, channel, glm::inverse(AiMatToGlmMat(node->mTransformation)));
            }
        }
    }
}
