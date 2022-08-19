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
        const float get_time(float factor = 1.0f) const
        {
            return time * factor;
        }
    };

    struct KeyRotation
    {
        glm::quat orientation;
        float time;
        const float get_time(float factor = 1.0f) const
        {
            return time * factor;
        }
    };

    struct KeyScale
    {
        glm::vec3 scale;
        float time;
        const float get_time(float factor = 1.0f) const
        {
            return time * factor;
        }
    };

    class Bone
    {
    public:
        Bone();
        Bone(const std::string &name, const aiNodeAnim *channel, const glm::mat4 &inverse_binding_pose);
        void update(float animation_time, float factor);
        glm::mat4 &get_local_transform(float animation_time, float factor);
        const std::string &get_name() const;
        float get_factor();
        void set_name(const std::string &name);
        void push_position(const glm::vec3 &pos, float time);
        void push_rotation(const glm::quat &quat, float time);
        void push_scale(const glm::vec3 &scale, float time);
        void replace_key_frame(const glm::mat4 &transform, float time);

    private:
        template <class T>
        const T &get_start(typename std::map<float, T> &mp, float animation_time, const T &default_value);
        template <class T>
        const T &get_end(typename std::map<float, T> &mp, float animation_time, const T &default_value);
        template <class T>
        int get_start_vec(typename std::vector<T> &vec, float animation_time);
        float get_scale_factor(float last_time_stamp, float next_time_stamp, float animation_time);
        glm::mat4 interpolate_position(float animation_time);
        glm::mat4 interpolate_rotation(float animation_time);
        glm::mat4 interpolate_scaling(float animation_time);

        std::map<float, KeyPosition> positions_;
        std::map<float, KeyRotation> rotations_;
        std::map<float, KeyScale> scales_;
        std::vector<KeyPosition> pos_;
        std::vector<KeyRotation> rot_;
        std::vector<KeyScale> scale_;

        std::string name_ = "";
        glm::mat4 local_transform_{1.0f};
        float factor_ = 1.0f;
    };

    template <class T>
    const T &Bone::get_start(typename std::map<float, T> &mp, float animation_time, const T &default_value)
    {
        typename std::map<float, T>::iterator lower = mp.lower_bound(animation_time);

        if (mp.size() != 0 && lower != mp.end() && animation_time == lower->first)
        {
            return lower->second;
        }
        if (lower == mp.begin() || mp.size() == 0)
        {
            return default_value;
        }
        return std::next(lower, -1)->second;
    }
    template <class T>
    const T &Bone::get_end(typename std::map<float, T> &mp, float animation_time, const T &default_value)
    {
        if (mp.size() == 0)
        {
            return default_value;
        }
        typename std::map<float, T>::iterator upper = mp.upper_bound(animation_time);
        if (upper == mp.begin())
        {
            return upper->second;
        }
        if (std::next(upper, -1)->first == animation_time || upper == mp.end())
        {
            return std::next(upper, -1)->second;
        }
        return upper->second;
    }
    template <class T>
    int Bone::get_start_vec(typename std::vector<T> &vec, float animation_time)
    {
        int size = vec.size() - 1;
        for (int index = 0; index < size; ++index)
        {
            if (animation_time < vec[index + 1].get_time(factor_))
            {
                return index;
            }
        }
        return size;
    }
}

#endif