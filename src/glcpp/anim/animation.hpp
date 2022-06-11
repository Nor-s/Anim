#ifndef GLCPP_ANIM_ANIMATION_HPP
#define GLCPP_ANIM_ANIMATION_HPP

#include <string>
#include <map>
#include <memory>
#include "bone.hpp"
#include <filesystem>

namespace glcpp
{
    enum AnimationType
    {
        None,
        Assimp,
        Json
    };
    class Animation
    {
    public:
        Animation() = default;
        Animation(const char *file_path)
            : name_(file_path)
        {
        }
        virtual ~Animation() = default;
        Bone *FindBone(const std::string &name)
        {
            auto iter = name_bone_map_.find(name);
            if (iter == name_bone_map_.end())
                return nullptr;
            else
                return &(*(iter->second));
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
        const AnimationType &get_type() const
        {
            return type;
        }
        virtual void reload()
        {
#ifndef NDEBUG
            std::cout << "reload:anim"<<std::endl;
#endif
        }
        void get_ai_animation(aiAnimation* ai_anim, const aiNode* ai_root_node) {
            std::filesystem::path p = std::filesystem::u8path(name_.c_str());
            std::string anim_name = p.filename().string();
            ai_anim->mName = aiString(anim_name);
            ai_anim->mDuration = static_cast<double>(duration_);
            ai_anim->mTicksPerSecond = static_cast<double>(ticks_per_second_);
            unsigned int size = name_bone_map_.size();
            aiNodeAnim** tmp_anim  = new aiNodeAnim * [size];
            
            int idx = 0;
            for (auto& name_bone : name_bone_map_) {
                const aiNode* node = ai_root_node->FindNode(name_bone.first.c_str());
                if (node) {
                      tmp_anim[idx] = new aiNodeAnim();
                    auto channel = tmp_anim[idx++];
                    name_bone.second->get_ai_node_anim(channel, node->mTransformation);
                }
            }
          
            ai_anim->mNumChannels = idx;
            ai_anim->mChannels = new aiNodeAnim * [idx];
            for (int i = 0; i < idx; i++) {
                ai_anim->mChannels[i] = new aiNodeAnim();
                ai_anim->mChannels[i] = tmp_anim[i];
            }
            delete[] tmp_anim;
    
        }
    protected:
        float duration_{0.0f};
        int ticks_per_second_{0};
        std::string name_;
        std::map<std::string, std::unique_ptr<Bone>> name_bone_map_;
        AnimationType type;
    };

}

#endif
