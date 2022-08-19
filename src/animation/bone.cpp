#include "bone.h"
#include "../util/utility.h"
#include <string>
#include <util/log.h>

namespace anim
{
    Bone::Bone() = default;
    Bone::Bone(const std::string &name, const aiNodeAnim *channel, const glm::mat4 &inverse_binding_pose)
        : name_(name),
          local_transform_(1.0f)
    {
        int num_positions = channel->mNumPositionKeys;
        int num_rotations = channel->mNumRotationKeys;
        int num_scales = channel->mNumScalingKeys;
        // push position
        for (int pos_idx = 0; pos_idx < num_positions; ++pos_idx)
        {
            aiVector3D aiPosition = channel->mPositionKeys[pos_idx].mValue;
            float time = channel->mPositionKeys[pos_idx].mTime;
            push_position(AiVecToGlmVec(aiPosition), time);
        }

        // push rotation
        for (int rot_idx = 0; rot_idx < num_rotations; ++rot_idx)
        {
            aiQuaternion aiOrientation = channel->mRotationKeys[rot_idx].mValue;
            float time = channel->mRotationKeys[rot_idx].mTime;
            push_rotation(AiQuatToGlmQuat(aiOrientation), time);
        }

        // push scaling
        for (int scale_idx = 0; scale_idx < num_scales; ++scale_idx)
        {
            aiVector3D scale = channel->mScalingKeys[scale_idx].mValue;
            float time = channel->mScalingKeys[scale_idx].mTime;
            push_scale(AiVecToGlmVec(scale), time);
        }

        // unbind bind pose
        for (auto time : time_set_)
        {
            auto t = positions_.find(time);
            auto r = rotations_.find(time);
            auto s = scales_.find(time);
            glm::vec3 tt = (t != positions_.end()) ? t->second.position : glm::vec3(0.0f, 0.0f, 0.0f);
            glm::quat rr = (r != rotations_.end()) ? r->second.orientation : glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
            glm::vec3 ss = (s != scales_.end()) ? s->second.scale : glm::vec3(1.0f, 1.0f, 1.0f);
            glm::mat4 transformation = glm::translate(glm::mat4(1.0f), tt) * glm::toMat4(glm::normalize(rr)) * glm::scale(glm::mat4(1.0f), ss);
            transformation = inverse_binding_pose * transformation;

            auto [translation, rotation, scale] = DecomposeTransform(transformation);
            if (t != positions_.end())
            {
                t->second.position = translation;
            }
            if (r != rotations_.end())
            {
                r->second.orientation = rotation;
            }
            if (s != scales_.end())
            {
                s->second.scale = scale;
            }
        }
    }

    void Bone::update(float animation_time, float factor)
    {
        factor_ = factor;
        glm::mat4 translation = interpolate_position(animation_time);
        glm::mat4 rotation = interpolate_rotation(animation_time);
        glm::mat4 scale = interpolate_scaling(animation_time);
        local_transform_ = translation * rotation * scale;
    }

    glm::mat4 &Bone::get_local_transform(float animation_time, float factor)
    {
        update(animation_time, factor);
        return local_transform_;
    }

    const std::set<float> &Bone::get_time_set() const
    {
        return time_set_;
    }

    const std::string &Bone::get_name() const { return name_; }

    float Bone::get_factor()
    {
        return factor_;
    }
    void Bone::set_name(const std::string &name)
    {
        name_ = name;
    }
    void Bone::push_position(const glm::vec3 &pos, float time)
    {
        positions_[time] = {pos, time};
        time_set_.insert(time);
    }
    void Bone::push_rotation(const glm::quat &quat, float time)
    {
        rotations_[time] = {quat, time};
        time_set_.insert(time);
    }
    void Bone::push_scale(const glm::vec3 &scale, float time)
    {
        scales_[time] = {scale, time};
        time_set_.insert(time);
    }

    float Bone::get_scale_factor(float last_time_stamp, float next_time_stamp, float animation_time)
    {
        float scaleFactor = 0.0f;
        float midWayLength = animation_time - last_time_stamp;
        float framesDiff = next_time_stamp - last_time_stamp;
        if (last_time_stamp == next_time_stamp || framesDiff < 0.0f)
        {
            return 1.0;
        }
        scaleFactor = midWayLength / framesDiff;
        return scaleFactor;
    }

    glm::mat4 Bone::interpolate_position(float animation_time)
    {
        // const KeyPosition &p1Index = get_end<KeyPosition>(positions_, animation_time, {glm::vec3(0.0f, 0.0f, 0.0f), animation_time});
        // const KeyPosition &p0Index = get_start<KeyPosition>(positions_, animation_time, {glm::vec3(0.0f, 0.0f, 0.0f), 0.0f});
        // int idx = get_start_vec<KeyPosition>(pos_, animation_time);
        // const KeyPosition &p0Index = pos_[idx];
        // const KeyPosition &p1Index = pos_[(idx == pos_.size() - 1) ? idx : idx + 1];
        const auto &[p0Index, p1Index] = get_start_end<KeyPosition>(positions_, animation_time, {glm::vec3(0.0f, 0.0f, 0.0f), 0.0f});
        float scaleFactor = get_scale_factor(p0Index.get_time(factor_),
                                             p1Index.get_time(factor_), animation_time);
        return glm::translate(glm::mat4(1.0f), glm::mix(p0Index.position, p1Index.position, scaleFactor));
    }

    glm::mat4 Bone::interpolate_rotation(float animation_time)
    {
        const auto &[p0Index, p1Index] = get_start_end<KeyRotation>(rotations_, animation_time, {glm::quat(1.0f, 0.0f, 0.0f, 0.0f), 0.0f});
        float scaleFactor = get_scale_factor(p0Index.get_time(factor_),
                                             p1Index.get_time(factor_), animation_time);
        glm::quat finalRotation = glm::slerp(p0Index.orientation, p1Index.orientation, scaleFactor);
        return glm::toMat4(glm::normalize(finalRotation));
    }

    glm::mat4 Bone::interpolate_scaling(float animation_time)
    {
        const auto &[p0Index, p1Index] = get_start_end<KeyScale>(scales_, animation_time, {glm::vec3(1.0f, 1.0f, 1.0f), 0.0f});
        float scaleFactor = get_scale_factor(p0Index.get_time(factor_),
                                             p1Index.get_time(factor_), animation_time);
        return glm::scale(glm::mat4(1.0f), glm::mix(p0Index.scale, p1Index.scale, scaleFactor));
    }
    void Bone::replace_key_frame(const glm::mat4 &transform, float time)
    {
        auto [t, r, s] = DecomposeTransform(transform);
        float time_stamp = floorf(time) / factor_;
        positions_[time_stamp] = {t, time_stamp};
        scales_[time_stamp] = {s, time_stamp};
        rotations_[time_stamp] = {r, time_stamp};
        time_set_.insert(time_stamp);
    }
}