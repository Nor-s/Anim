#ifndef GLCPP_ANIM_ANIMATION_HPP
#define GLCPP_ANIM_ANIMATION_HPP

#include <string>
#include <map>
#include <memory>
namespace glcpp
{
    class Bone;
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
        }

    protected:
        float duration_;
        int ticks_per_second_;
        std::string name_;
        std::map<std::string, std::unique_ptr<Bone>> name_bone_map_;
        AnimationType type;
    };

}

#endif
