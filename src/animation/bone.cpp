#include "bone.h"
#include "../util/utility.h"
#include <string>

namespace anim
{
    Bone::Bone() = default;
    Bone::Bone(const std::string &name, const aiNodeAnim *channel, const glm::mat4 &inverse_binding_pose)
        : name_(name),
          local_transform_(1.0f)
    {
        std::set<float> time_set;
        num_positions_ = channel->mNumPositionKeys;

        for (int pos_idx = 0; pos_idx < num_positions_; ++pos_idx)
        {
            aiVector3D aiPosition = channel->mPositionKeys[pos_idx].mValue;
            float time = channel->mPositionKeys[pos_idx].mTime;
            KeyPosition data;
            data.position = AiVecToGlmVec(aiPosition);
            data.time = time;
            positions_.push_back(data);
            time_positions_map_[time] = pos_idx;
            time_set.insert(time);
        }

        num_rotations_ = channel->mNumRotationKeys;
        for (int rot_idx = 0; rot_idx < num_rotations_; ++rot_idx)
        {
            aiQuaternion aiOrientation = channel->mRotationKeys[rot_idx].mValue;
            float time = channel->mRotationKeys[rot_idx].mTime;
            KeyRotation data;
            data.orientation = AiQuatToGlmQuat(aiOrientation);
            data.time = time;
            rotations_.push_back(data);
            time_rotations_map_[time] = rot_idx;
            time_set.insert(time);
        }

        num_scales_ = channel->mNumScalingKeys;
        for (int scale_idx = 0; scale_idx < num_scales_; ++scale_idx)
        {
            aiVector3D scale = channel->mScalingKeys[scale_idx].mValue;
            float time = channel->mScalingKeys[scale_idx].mTime;
            KeyScale data;
            data.scale = AiVecToGlmVec(scale);
            data.time = time;
            scales_.push_back(data);
            time_scales_map_[time] = scale_idx;
            time_set.insert(time);
        }
        time_list_.reserve(time_set.size());

        for (auto time : time_set)
        {
            auto it_pose = time_positions_map_.find(time);
            auto it_scale = time_scales_map_.find(time);
            auto it_rotate = time_rotations_map_.find(time);
            glm::vec3 v_scale = (it_scale != time_scales_map_.end()) ? scales_[it_scale->second].scale : glm::vec3(1.0f, 1.0f, 1.0f);
            glm::vec3 v_pose = (it_pose != time_positions_map_.end()) ? positions_[it_pose->second].position : glm::vec3(0.0f, 0.0f, 0.0f);
            glm::quat v_rotate = (it_rotate != time_rotations_map_.end()) ? rotations_[it_rotate->second].orientation : glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
            glm::mat4 transformation = glm::translate(glm::mat4(1.0f), v_pose) * glm::toMat4(glm::normalize(v_rotate)) * glm::scale(glm::mat4(1.0f), v_scale);
            transformation = inverse_binding_pose * transformation;

            auto [translation, rotation, scale] = DecomposeTransform(transformation);
            if (it_pose != time_positions_map_.end())
            {
                positions_[it_pose->second].position = translation;
            }
            if (it_rotate != time_rotations_map_.end())
            {
                rotations_[it_rotate->second].orientation = rotation;
            }
            if (it_scale != time_scales_map_.end())
            {
                scales_[it_scale->second].scale = scale;
            }
            time_list_.push_back(time);
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

    glm::mat4 &Bone::get_local_transform() { return local_transform_; }

    glm::mat4 &Bone::get_local_transform(float animation_time, float factor)
    {
        update(animation_time, factor);
        return local_transform_;
    }

    const std::string &Bone::get_bone_name() const { return name_; }

    int Bone::get_position_index(float animation_time)
    {
        int size = num_positions_ - 1;
        for (int index = 0; index < size; ++index)
        {
            if (animation_time < positions_[index + 1].get_time(factor_))
            {
                recently_used_position_idx_ = index;
                return index;
            }
        }
        return size;
    }

    int Bone::get_rotation_index(float animation_time)
    {
        int size = num_rotations_ - 1;
        for (int index = 0; index < size; ++index)
        {
            if (animation_time < rotations_[index + 1].get_time(factor_))
            {
                recently_used_rotation_idx_ = index;
                return index;
            }
        }
        return size;
    }

    int Bone::GetScaleIndex(float animation_time)
    {
        int size = num_scales_ - 1;
        for (int index = 0; index < size; ++index)
        {
            if (animation_time < scales_[index + 1].get_time(factor_))
            {
                recently_used_scale_idx_ = index;
                return index;
            }
        }
        return size;
    }

    std::vector<float> &Bone::get_mutable_time_list()
    {
        return time_list_;
    }

    float Bone::get_factor()
    {
        return factor_;
    }
    glm::vec3 *Bone::get_mutable_pointer_positions(float time)
    {
        auto it = time_positions_map_.find(time);
        if (it == time_positions_map_.end())
        {
            return nullptr;
        }
        return &(positions_[it->second].position);
    }
    glm::quat *Bone::get_mutable_pointer_rotations(float time)
    {
        auto it = time_rotations_map_.find(time);
        if (it == time_rotations_map_.end())
        {
            return nullptr;
        }
        return &(rotations_[it->second].orientation);
    }
    glm::vec3 *Bone::get_mutable_pointer_scales(float time)
    {
        auto it = time_scales_map_.find(time);
        if (it == time_scales_map_.end())
        {
            return nullptr;
        }
        return &(scales_[it->second].scale);
    }
    glm::vec3 *Bone::get_mutable_pointer_recently_used_position()
    {
        if (positions_.size() == 0)
        {
            return nullptr;
        }
        return &(positions_[recently_used_position_idx_].position);
    }
    glm::quat *Bone::get_mutable_pointer_recently_used_rotation()
    {
        if (rotations_.size() == 0)
        {
            return nullptr;
        }
        return &(rotations_[recently_used_rotation_idx_].orientation);
    }
    glm::vec3 *Bone::get_mutable_pointer_recently_used_scale()
    {
        if (scales_.size() == 0)
        {
            return nullptr;
        }
        return &(scales_[recently_used_scale_idx_].scale);
    }
    void Bone::set_name(const std::string &name)
    {
        name_ = name;
    }
    void Bone::push_position(const glm::vec3 &pos, float time)
    {
        if (positions_.empty() || positions_.back().time <= time)
        {
            positions_.push_back({pos, time});
            time_positions_map_[time] = positions_.size() - 1;
            num_positions_++;
        }
    }
    void Bone::push_rotation(const glm::quat &quat, float time)
    {
        if (rotations_.empty() || rotations_.back().time <= time)
        {
            // quat = glm::normalize(quat);
            rotations_.push_back({quat, time});
            time_rotations_map_[time] = rotations_.size() - 1;
            num_rotations_++;
        }
    }
    void Bone::push_scale(const glm::vec3 &scale, float time)
    {
        if (scales_.empty() || scales_.back().time <= time)
        {
            scales_.push_back({scale, time});
            time_scales_map_[time] = scales_.size() - 1;
            num_scales_++;
        }
    }
    void Bone::init_time_list()
    {
        std::set<float> time_set;
        std::transform(time_scales_map_.begin(), time_scales_map_.end(),
                       std::inserter(time_set, time_set.begin()),
                       [](const std::pair<float, int> &key_value)
                       { return key_value.first; });
        std::transform(time_rotations_map_.begin(), time_rotations_map_.end(),
                       std::inserter(time_set, time_set.begin()),
                       [](const std::pair<float, int> &key_value)
                       { return key_value.first; });
        std::transform(time_scales_map_.begin(), time_scales_map_.end(),
                       std::inserter(time_set, time_set.begin()),
                       [](const std::pair<float, int> &key_value)
                       { return key_value.first; });
        time_list_.clear();
        time_list_.reserve(time_set.size());
        for (auto &time : time_set)
        {
            time_list_.push_back(time);
        }
    }
    void Bone::get_ai_node_anim(aiNodeAnim *channel, const aiMatrix4x4 &binding_pose_transform)
    {
        channel->mNodeName = aiString(name_);
        channel->mNumPositionKeys = positions_.size();
        channel->mNumRotationKeys = rotations_.size();
        channel->mNumScalingKeys = scales_.size();

        channel->mPositionKeys = new aiVectorKey[channel->mNumPositionKeys];
        channel->mRotationKeys = new aiQuatKey[channel->mNumRotationKeys];
        channel->mScalingKeys = new aiVectorKey[channel->mNumScalingKeys];

        unsigned int pos_idx = 0, rot_idx = 0, scale_idx = 0;

        for (auto time : time_list_)
        {
            auto it_pose = time_positions_map_.find(time);
            auto it_scale = time_scales_map_.find(time);
            auto it_rotate = time_rotations_map_.find(time);
            glm::vec3 v_scale = (it_scale != time_scales_map_.end()) ? scales_[it_scale->second].scale : glm::vec3(1.0f, 1.0f, 1.0f);
            glm::vec3 v_pose = (it_pose != time_positions_map_.end()) ? positions_[it_pose->second].position : glm::vec3(0.0f, 0.0f, 0.0f);
            glm::quat v_rotate = (it_rotate != time_rotations_map_.end()) ? rotations_[it_rotate->second].orientation : glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
            glm::mat4 transformation = glm::translate(glm::mat4(1.0f), v_pose) * glm::toMat4(glm::normalize(v_rotate)) * glm::scale(glm::mat4(1.0f), v_scale);
            transformation = AiMatToGlmMat(binding_pose_transform) * transformation;

            auto [t, r, s] = DecomposeTransform(transformation);

            if (it_pose != time_positions_map_.end())
            {
                channel->mPositionKeys[pos_idx].mValue = GlmVecToAiVec(t);
                channel->mPositionKeys[pos_idx].mTime = time;
                pos_idx++;
            }
            if (it_rotate != time_rotations_map_.end())
            {
                channel->mRotationKeys[rot_idx].mValue = GlmQuatToAiQuat(r);
                channel->mRotationKeys[rot_idx].mTime = time;
                rot_idx++;
            }
            if (it_scale != time_scales_map_.end())
            {
                channel->mScalingKeys[scale_idx].mValue = GlmVecToAiVec(s);
                channel->mScalingKeys[scale_idx].mTime = time;
                scale_idx++;
            }
        }
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
        int p0Index = get_position_index(animation_time);
        int p1Index = num_positions_ - 1;
        // for nested frame
        auto it = time_positions_map_.find(positions_[p0Index].time);
        it++;
        if (it != time_positions_map_.end())
            p1Index = it->second;
        float scaleFactor = get_scale_factor(positions_[p0Index].get_time(factor_),
                                             positions_[p1Index].get_time(factor_), animation_time);
        glm::vec3 finalPosition = glm::mix(positions_[p0Index].position, positions_[p1Index].position, scaleFactor);

        return glm::translate(glm::mat4(1.0f), finalPosition);
    }

    glm::mat4 Bone::interpolate_rotation(float animation_time)
    {
        int p0Index = get_rotation_index(animation_time);
        int p1Index = num_rotations_ - 1;
        // for nested frame
        auto it = time_rotations_map_.find(rotations_[p0Index].time);
        it++;
        if (it != time_rotations_map_.end())
            p1Index = it->second;
        float scaleFactor = get_scale_factor(rotations_[p0Index].get_time(factor_),
                                             rotations_[p1Index].get_time(factor_), animation_time);
        glm::quat finalRotation = glm::slerp(rotations_[p0Index].orientation, rotations_[p1Index].orientation, scaleFactor);
        finalRotation = glm::normalize(finalRotation);
        return glm::toMat4(finalRotation);
    }

    glm::mat4 Bone::interpolate_scaling(float animation_time)
    {
        int p0Index = GetScaleIndex(animation_time);
        int p1Index = num_scales_ - 1;
        // for nested frame
        auto it = time_scales_map_.find(scales_[p0Index].time);
        it++;
        if (it != time_scales_map_.end())
            p1Index = it->second;
        float scaleFactor = get_scale_factor(scales_[p0Index].get_time(factor_),
                                             scales_[p1Index].get_time(factor_), animation_time);
        glm::vec3 finalScale = glm::mix(scales_[p0Index].scale, scales_[p1Index].scale, scaleFactor);
        return glm::scale(glm::mat4(1.0f), finalScale);
    }
}