#include "json_animation.h"

#include <fstream>
#include <filesystem>
#include "bone.h"

namespace anim
{
    JsonAnimation::JsonAnimation(const char *animation_path)
        : Animation(animation_path)
    {
        init();
    }
    JsonAnimation::~JsonAnimation()
    {
        name_bone_map_.clear();
    }

    void JsonAnimation::reload()
    {
#ifndef NDEBUG
        std::cout << "relaod: json" << std::endl;
#endif
        destroy();
        init();
    }
    void JsonAnimation::destroy()
    {
        name_bone_map_.clear();
    }
    void JsonAnimation::init()
    {
        try
        {
            std::filesystem::path path = std::filesystem::u8path((path_).c_str());
            name_ = path.filename().string();
            type_ = AnimationType::Json;
            Json::Value root;
#ifndef NDEBUG
            std::cout << "init json: " << path_ << std::endl;
#endif
            std::ifstream anim_stream(path.c_str(), std::ifstream::in | std::ifstream::binary);
            anim_stream >> root;
            if (anim_stream.is_open())
            {
                anim_stream.close();
            }
            duration_ = root.get("duration", "0").asFloat();
            ticks_per_second_ = root.get("ticksPerSecond", "1").asFloat();
            const Json::Value frames = root["frames"];
            if (frames != "null")
            {
                process_frames(frames);
            }
        }
        catch (std::exception &e)
        {
            type_ = AnimationType::None;
#ifndef NDEBUG
            std::cout << e.what() << std::endl;
#endif
        }
    }
    void JsonAnimation::process_frames(const Json::Value &frames)
    {
        try
        {
            uint32_t size = static_cast<uint32_t>(frames.size());
            for (uint32_t idx = 0; idx < size; idx++)
            {
                float time = frames[idx].get("time", "0").asFloat();
                const auto &bones = frames[idx]["bones"];
                if (bones != "null")
                {
                    process_bones(bones, time);
                }
            }
            for (auto &name_bone : name_bone_map_)
            {
                name_bone.second->init_time_list();
            }
        }
        catch (std::exception &e)
        {
#ifndef NDEBUG
            std::cout << e.what() << std::endl;
#endif
        }
    }
    void JsonAnimation::process_bones(const Json::Value &bones, float time)
    {
        try
        {
            uint32_t size = static_cast<uint32_t>(bones.size());

            for (uint32_t idx = 0; idx < size; idx++)
            {
                Bone *bone;
                std::string bone_name = bones[idx].get("name", "").asString();
                auto it = name_bone_map_.find(bone_name);
                if (it != name_bone_map_.end())
                {
                    bone = it->second.get();
                }
                else
                {
                    name_bone_map_[bone_name] = std::make_unique<Bone>();
                    bone = name_bone_map_[bone_name].get();
                    bone->set_name(bone_name);
                }
                bone->push_position(get_position(bones[idx]["position"]), time);
                bone->push_rotation(get_rotation(bones[idx]["rotation"]), time);
                bone->push_scale(get_scale(bones[idx]["scale"]), time);
            }
        }
        catch (std::exception &e)
        {
#ifndef NDEBUG
            std::cout << e.what() << std::endl;
#endif
        }
    }
    glm::vec3 JsonAnimation::get_position(const Json::Value &bone)
    {
        if (bone == "null")
        {
            return glm::vec3(0.0f, 0.0f, 0.0f);
        }
        return glm::vec3(bone.get("x", "0.0").asFloat(), bone.get("y", "0.0").asFloat(), bone.get("z", "0.0").asFloat());
    }
    glm::quat JsonAnimation::get_rotation(const Json::Value &bone)
    {
        if (bone == "null")
        {
            return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        }
        return glm::quat(bone.get("w", "0.0").asFloat(), bone.get("x", "0.0").asFloat(), bone.get("y", "0.0").asFloat(), bone.get("z", "0.0").asFloat());
    }
    glm::vec3 JsonAnimation::get_scale(const Json::Value &bone)
    {
        if (bone == "null")
        {
            return glm::vec3(1.0f, 1.0f, 1.0f);
        }
        return glm::vec3(bone.get("x", "0.0").asFloat(), bone.get("y", "0.0").asFloat(), bone.get("z", "0.0").asFloat());
    }
}