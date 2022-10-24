/**
  // json 형태
{
    "fileName": "sample/mixamo_dance.gif",
    "duration": 1,
    "ticksPerSecond": 1,
    "bindpose": {
      "bone_name1": [
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
      ],
      "bone_name2": [
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
      ]
      ...
    },
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
#ifndef ANIM_ANIMATION_JSON_ANIMATION_H
#define ANIM_ANIMATION_JSON_ANIMATION_H

#include <json/json.h>
#include <fstream>
#include "animation.h"
#include <glm/glm.hpp>

namespace anim
{
  class JsonAnimation : public Animation
  {
  public:
    JsonAnimation() = delete;
    JsonAnimation(const char *animation_path);

    ~JsonAnimation();

    void reload() override;

  private:
    void destroy();

    void init();

    void process_frames(const Json::Value &frames);

    void process_bones(const Json::Value &bones, float time);

    void process_bindpose(const Json::Value &bindpose);

    glm::vec3 get_position(const Json::Value &bone);

    glm::quat get_rotation(const Json::Value &bone);

    glm::vec3 get_scale(const Json::Value &bone);
    glm::mat4 get_mat(const Json::Value &mat_array);
  };
}
#endif