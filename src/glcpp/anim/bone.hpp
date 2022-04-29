#ifndef GLCPP_ANIM_BONE_HPP
#define GLCPP_ANIM_BONE_HPP

#include <vector>
#include <assimp/scene.h>
#include <list>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "../utility.hpp"

namespace glcpp
{
    struct KeyPosition
    {
        glm::vec3 position;
        float time;
        float get_time(float factor = 1.0f)
        {
            return (float)static_cast<uint32_t>(roundf(time * factor));
        }
    };

    struct KeyRotation
    {
        glm::quat orientation;
        float time;
        float get_time(float factor = 1.0f)
        {
            return (float)static_cast<uint32_t>(roundf(time * factor));
        }
    };

    struct KeyScale
    {
        glm::vec3 scale;
        float time;
        float get_time(float factor = 1.0f)
        {
            return (float)static_cast<uint32_t>(roundf(time * factor));
        }
    };

    class Bone
    {
    public:
        Bone(const std::string &name, const aiNodeAnim *channel, const glm::mat4 &inverse_binding_pose)
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
                glm::vec3 v_pose = (it_pose != time_positions_map_.end()) ? positions_[it_pose->second].position : glm::vec3(0.0f, 0.0f, 0.0f);
                glm::vec3 v_scale = (it_scale != time_scales_map_.end()) ? scales_[it_scale->second].scale : glm::vec3(1.0f, 1.0f, 1.0f);
                glm::quat v_rotate = (it_rotate != time_rotations_map_.end()) ? rotations_[it_rotate->second].orientation : glm::vec3(0.0f, 0.0f, 0.0f);
                glm::mat4 transformation = glm::translate(glm::mat4(1.0f), v_pose) * glm::toMat4(glm::normalize(v_rotate)) * glm::scale(glm::mat4(1.0f), v_scale);
                transformation = inverse_binding_pose * transformation;

                glm::vec3 scale;
                glm::quat rotation;
                glm::vec3 translation;
                glm::vec3 skew;
                glm::vec4 perspective;
                glm::decompose(transformation, scale, rotation, translation, skew, perspective);
                scales_[it_scale->second].scale = scale;
                positions_[it_pose->second].position = translation;
                rotations_[it_rotate->second].orientation = rotation;
                time_list_.push_back(time);
            }
        }

        void
        Update(float animationTime, float factor)
        {
            glm::mat4 translation = InterpolatePosition(animationTime);
            glm::mat4 rotation = InterpolateRotation(animationTime);
            glm::mat4 scale = InterpolateScaling(animationTime);
            local_transform_ = translation * rotation * scale;
            factor_ = factor;
        }

        glm::mat4 &GetLocalTransform() { return local_transform_; }

        const std::string &get_bone_name() const { return name_; }

        int GetPositionIndex(float animationTime)
        {

            for (int index = 0; index < num_positions_ - 1; ++index)
            {
                if (animationTime < positions_[index + 1].get_time(factor_))
                    return index;
            }
            return 0;
        }

        int GetRotationIndex(float animationTime)
        {
            for (int index = 0; index < num_rotations_ - 1; ++index)
            {
                if (animationTime < rotations_[index + 1].get_time(factor_))
                    return index;
            }
            return 0;
        }

        int GetScaleIndex(float animationTime)
        {

            for (int index = 0; index < num_scales_ - 1; ++index)
            {
                if (animationTime < scales_[index + 1].get_time(factor_))
                    return index;
            }
            return 0;
        }

        std::vector<float> &get_mutable_time_list()
        {
            return time_list_;
        }

        float get_factor()
        {
            return factor_;
        }

    private:
        float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
        {
            float scaleFactor = 0.0f;
            float midWayLength = animationTime - lastTimeStamp;
            float framesDiff = nextTimeStamp - lastTimeStamp;
            scaleFactor = midWayLength / framesDiff;
            return scaleFactor;
        }

        glm::mat4 InterpolatePosition(float animationTime)
        {
            if (1 == num_positions_)
                return glm::translate(glm::mat4(1.0f), positions_[0].position);

            int p0Index = GetPositionIndex(animationTime);
            int p1Index = p0Index + 1;
            float scaleFactor = GetScaleFactor(positions_[p0Index].get_time(factor_),
                                               positions_[p1Index].get_time(factor_), animationTime);
            glm::vec3 finalPosition = glm::mix(positions_[p0Index].position, positions_[p1Index].position, scaleFactor);
            return glm::translate(glm::mat4(1.0f), finalPosition);
        }

        glm::mat4 InterpolateRotation(float animationTime)
        {
            if (1 == num_rotations_)
            {
                auto rotation = glm::normalize(rotations_[0].orientation);
                return glm::toMat4(rotation);
            }

            int p0Index = GetRotationIndex(animationTime);
            int p1Index = p0Index + 1;
            float scaleFactor = GetScaleFactor(rotations_[p0Index].get_time(factor_),
                                               rotations_[p1Index].get_time(factor_), animationTime);
            glm::quat finalRotation = glm::slerp(rotations_[p0Index].orientation, rotations_[p1Index].orientation, scaleFactor);
            finalRotation = glm::normalize(finalRotation);
            return glm::toMat4(finalRotation);
        }

        glm::mat4 InterpolateScaling(float animationTime)
        {
            if (1 == num_scales_)
                return glm::scale(glm::mat4(1.0f), scales_[0].scale);

            int p0Index = GetScaleIndex(animationTime);
            int p1Index = p0Index + 1;
            float scaleFactor = GetScaleFactor(scales_[p0Index].get_time(factor_),
                                               scales_[p1Index].get_time(factor_), animationTime);
            glm::vec3 finalScale = glm::mix(scales_[p0Index].scale, scales_[p1Index].scale, scaleFactor);
            return glm::scale(glm::mat4(1.0f), finalScale);
        }

        std::vector<KeyPosition> positions_;
        std::vector<KeyRotation> rotations_;
        std::vector<KeyScale> scales_;

        int num_positions_;
        int num_rotations_;
        int num_scales_;

        std::string name_;
        glm::mat4 local_transform_;
        float factor_;

        // keyframe, position, rotation, scale
        std::map<float, int> time_positions_map_;
        std::map<float, int> time_rotations_map_;
        std::map<float, int> time_scales_map_;
        std::vector<float> time_list_;
    };
}
#endif