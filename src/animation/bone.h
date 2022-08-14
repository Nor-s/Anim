#ifndef ANIM_ANIMATION_BONE_H
#define ANIM_ANIMATION_BONE_H

#include <vector>
#include <assimp/scene.h>
#include <list>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <set>
#include <iterator>
#include <map>

namespace anim
{
    struct KeyPosition
    {
        glm::vec3 position;
        float time;
        float get_time(float factor = 1.0f)
        {
            return time * factor; // (float)static_cast<uint32_t>(roundf(time * factor));
        }
    };

    struct KeyRotation
    {
        glm::quat orientation;
        float time;
        float get_time(float factor = 1.0f)
        {
            return time * factor; //(float)static_cast<uint32_t>(roundf(time * factor));
        }
    };

    struct KeyScale
    {
        glm::vec3 scale;
        float time;
        float get_time(float factor = 1.0f)
        {
            return time * factor; //(float)static_cast<uint32_t>(roundf(time * factor));
        }
    };

    class Bone
    {
    public:
        Bone();
        Bone(const std::string &name, const aiNodeAnim *channel, const glm::mat4 &inverse_binding_pose);
        void update(float animation_time, float factor);
        glm::mat4 &get_local_transform();
        glm::mat4 &get_local_transform(float animation_time, float factor);
        const std::string &get_bone_name() const;
        int get_position_index(float animation_time);
        int get_rotation_index(float animation_time);
        int GetScaleIndex(float animation_time);
        std::vector<float> &get_mutable_time_list();
        float get_factor();
        glm::vec3 *get_mutable_pointer_positions(float time);
        glm::quat *get_mutable_pointer_rotations(float time);
        glm::vec3 *get_mutable_pointer_scales(float time);
        glm::vec3 *get_mutable_pointer_recently_used_position();
        glm::quat *get_mutable_pointer_recently_used_rotation();
        glm::vec3 *get_mutable_pointer_recently_used_scale();
        void set_name(const std::string &name);
        void push_position(const glm::vec3 &pos, float time);
        void push_rotation(const glm::quat &quat, float time);
        void push_scale(const glm::vec3 &scale, float time);
        void init_time_list();
        void get_ai_node_anim(aiNodeAnim *channel, const aiMatrix4x4 &binding_pose_transform);

    private:
        float get_scale_factor(float last_time_stamp, float next_time_stamp, float animation_time);
        glm::mat4 interpolate_position(float animation_time);
        glm::mat4 interpolate_rotation(float animation_time);
        glm::mat4 interpolate_scaling(float animation_time);

        std::vector<KeyPosition> positions_;
        std::vector<KeyRotation> rotations_;
        std::vector<KeyScale> scales_;

        int num_positions_ = 0;
        int num_rotations_ = 0;
        int num_scales_ = 0;

        std::string name_ = "";
        glm::mat4 local_transform_{1.0f};
        float factor_ = 1.0f;

        // keyframe, position, rotation, scale
        std::map<float, int> time_positions_map_;
        std::map<float, int> time_rotations_map_;
        std::map<float, int> time_scales_map_;
        std::vector<float> time_list_;

        uint32_t recently_used_position_idx_ = 0;
        uint32_t recently_used_rotation_idx_ = 0;
        uint32_t recently_used_scale_idx_ = 0;
    };
}

#endif