#ifndef ANIM_ANIMATION_ANIMATION_H
#define ANIM_ANIMATION_ANIMATION_H

#include <assimp/scene.h>

#include <string>
#include <map>
#include <filesystem>
#include <iostream>

#include <memory>
#include "bone.h"
namespace anim
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
        Animation(const char *file_path);
        virtual ~Animation() = default;
        Bone *find_bone(const std::string &name);
        float get_fps();
        float get_duration();
        float get_current_duration();
        const std::string &get_name() const;
        const char *get_path() const;
        const std::map<std::string, std::unique_ptr<Bone>> &get_name_bone_map() const;
        std::map<std::string, std::unique_ptr<Bone>> &get_mutable_name_bone_map();
        const AnimationType &get_type() const;
        virtual void reload();
        void get_ai_animation(aiAnimation *ai_anim, const aiNode *ai_root_node, float factor = 1.0, bool is_linear = true);
        void set_id(int id);
        const int get_id() const;
        void add_and_replace_bone(const std::string &name, const glm::mat4 &transform, float time);
        void sub_bone(const std::string &name, float time);
        void replace_bone(const std::string &name, const glm::mat4 &transform, float time);

    protected:
        float duration_{0.0f};
        int fps_{0};
        std::string name_{};
        std::map<std::string, std::unique_ptr<Bone>> name_bone_map_{};
        AnimationType type_{};
        std::string path_{};
        int id_{-1};
    };

}

#endif