/**
  // json 형태
{
    "fileName": "sample/mixamo_dance.gif",
    "duration": 1,
    "ticksPerSecond": 1,
    "frames": [
        {
            "time": 0,
            "bones": [
                {
                    "name": "neck",
                    "rotation": {
                        "w": 1.0,
                        "x": 0.1,
                        "y": 0.2,
                        "z": 0.3
                    },
                    "position": {
                        "x": 0.0,
                        "y": 0.0,
                        "z": 0.0
                    },
                    "scale": {
                        "x": 1.0,
                        "y": 1.0,
                        "z": 1.0
                    }
                }
            ]
        }
    ]
}
 */
#ifndef GLCPP_ANIM_JSON_ANIMATION_HPP
#define GLCPP_ANIM_JSON_ANIMATION_HPP

#include <json/json.h>
#include <fstream>
#include "animation.hpp"

namespace glcpp
{
  class JsonAnimation : public Animation
  {
  public:
    JsonAnimation() = default;
    JsonAnimation(const char *animation_path)
    {
      type = AnimationType::Json;
      Json::Value root;
      std::ifstream anim_stream(animation_path, std::ifstream::binary);
      anim_stream >> root;

      name_ = std::string(animation_path); // root.get("fileName", "").asString();
      duration_ = root.get("duration", "0").asFloat();
      ticks_per_second_ = root.get("ticksPerSecond", "1").asFloat();
      const Json::Value frames = root["frames"];
      if (frames != 'null')
      {
        process_frames(frames);
      }
    }

  private:
    void process_frames(const Json::Value &frames)
    {
      for (int idx = 0; idx < frames.size(); idx++)
      {
        float time = frames[idx].get("time", "0").asFloat();
        const auto &bones = frames[idx]["bones"];
        if (bones != 'null')
        {
          process_bones(bones, time);
        }
      }
      for (auto &name_bone : name_bone_map_)
      {
        name_bone.second->init_time_list();
      }
    }
    void process_bones(const Json::Value &bones, float time)
    {
      for (int idx = 0; idx < bones.size(); idx++)
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
    glm::vec3 get_position(const Json::Value &bone)
    {
      if (bone == 'null')
      {
        return glm::vec3(0.0f, 0.0f, 0.0f);
      }
      return glm::vec3(bone.get("x", "0.0").asFloat(), bone.get("y", "0.0").asFloat(), bone.get("z", "0.0").asFloat());
    }
    glm::quat get_rotation(const Json::Value &bone)
    {
      if (bone == 'null')
      {
        return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
      }
      return glm::quat(bone.get("w", "0.0").asFloat(), bone.get("x", "0.0").asFloat(), bone.get("y", "0.0").asFloat(), bone.get("z", "0.0").asFloat());
    }
    glm::vec3 get_scale(const Json::Value &bone)
    {
      if (bone == 'null')
      {
        return glm::vec3(1.0f, 1.0f, 1.0f);
      }
      return glm::vec3(bone.get("x", "0.0").asFloat(), bone.get("y", "0.0").asFloat(), bone.get("z", "0.0").asFloat());
    }
  };
}

#endif