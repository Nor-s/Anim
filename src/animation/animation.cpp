#include "animation.h"
#include <string>
#include <map>
#include <filesystem>
#include <iostream>

namespace anim
{
    Animation::Animation(const char *file_path)
        : path_(file_path)
    {
    }
    Bone *Animation::find_bone(const std::string &name)
    {
        auto iter = name_bone_map_.find(name);
        if (iter == name_bone_map_.end())
            return nullptr;
        else
            return &(*(iter->second));
    }
    inline float Animation::get_ticks_per_second() { return ticks_per_second_; }
    inline float Animation::get_duration() { return duration_; }
    const std::string &Animation::get_name() const
    {
        return name_;
    }
    const char *Animation::get_path() const
    {
        return path_.c_str();
    }
    const std::map<std::string, std::unique_ptr<Bone>> &Animation::get_name_bone_map() const
    {
        return name_bone_map_;
    }
    std::map<std::string, std::unique_ptr<Bone>> &Animation::get_mutable_name_bone_map()
    {
        return name_bone_map_;
    }
    const AnimationType &Animation::get_type() const
    {
        return type_;
    }
    void Animation::reload()
    {
#ifndef NDEBUG
        std::cout << "reload:anim" << std::endl;
#endif
    }
    void Animation::get_ai_animation(aiAnimation *ai_anim, const aiNode *ai_root_node)
    {
        std::filesystem::path p = std::filesystem::u8path(path_.c_str());
        std::string anim_name = p.filename().string();
        ai_anim->mName = aiString(anim_name);
        ai_anim->mDuration = static_cast<double>(duration_);
        ai_anim->mTicksPerSecond = static_cast<double>(ticks_per_second_);
        unsigned int size = name_bone_map_.size();
        aiNodeAnim **tmp_anim = new aiNodeAnim *[size];

        int idx = 0;
        for (auto &name_bone : name_bone_map_)
        {
            const aiNode *node = ai_root_node->FindNode(name_bone.first.c_str());
            if (node)
            {
                tmp_anim[idx] = new aiNodeAnim();
                auto channel = tmp_anim[idx++];
                name_bone.second->get_ai_node_anim(channel, node->mTransformation);
            }
        }

        ai_anim->mNumChannels = idx;
        ai_anim->mChannels = new aiNodeAnim *[idx];
        for (int i = 0; i < idx; i++)
        {
            ai_anim->mChannels[i] = new aiNodeAnim();
            ai_anim->mChannels[i] = tmp_anim[i];
        }
        delete[] tmp_anim;
    }
}