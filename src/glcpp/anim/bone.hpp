#ifndef GLCPP_ANIM_BONE_HPP
#define GLCPP_ANIM_BONE_HPP

#include <vector>
#include <assimp/scene.h>
#include <list>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include "../utility.hpp"

namespace glcpp
{
    struct KeyPosition
    {
        glm::vec3 position;
        float timeStamp;
    };

    struct KeyRotation
    {
        glm::quat orientation;
        float timeStamp;
    };

    struct KeyScale
    {
        glm::vec3 scale;
        float timeStamp;
    };

    class Bone
    {
    public:
        Bone(const std::string &name, const aiNodeAnim *channel)
            : name_(name),
              local_transform_(1.0f)
        {
            num_positions_ = channel->mNumPositionKeys;

            for (int positionIndex = 0; positionIndex < num_positions_; ++positionIndex)
            {
                aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
                float timeStamp = channel->mPositionKeys[positionIndex].mTime;
                KeyPosition data;
                data.position = AiVecToGlmVec(aiPosition);
                data.timeStamp = timeStamp;
                std::cout << timeStamp << "--\n";
                positions_.push_back(data);
            }

            num_rotations_ = channel->mNumRotationKeys;
            for (int rotationIndex = 0; rotationIndex < num_rotations_; ++rotationIndex)
            {
                aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
                float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
                KeyRotation data;
                data.orientation = AiQuatToGlmQuat(aiOrientation);
                data.timeStamp = timeStamp;
                rotations_.push_back(data);
                std::cout << timeStamp << "-----\n";
            }

            num_scalings_ = channel->mNumScalingKeys;
            for (int keyIndex = 0; keyIndex < num_scalings_; ++keyIndex)
            {
                aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
                float timeStamp = channel->mScalingKeys[keyIndex].mTime;
                KeyScale data;
                data.scale = AiVecToGlmVec(scale);
                data.timeStamp = timeStamp;
                scales_.push_back(data);
            }
        }

        void Update(float animationTime)
        {
            glm::mat4 translation = InterpolatePosition(animationTime);
            glm::mat4 rotation = InterpolateRotation(animationTime);
            glm::mat4 scale = InterpolateScaling(animationTime);
            local_transform_ = translation * rotation * scale;
        }

        glm::mat4 &GetLocalTransform() { return local_transform_; }

        std::string GetBoneName() const { return name_; }

        int GetPositionIndex(float animationTime)
        {
            for (int index = 0; index < num_positions_ - 1; ++index)
            {
                if (animationTime < positions_[index + 1].timeStamp)
                    return index;
            }
            return 0;
        }

        int GetRotationIndex(float animationTime)
        {
            for (int index = 0; index < num_rotations_ - 1; ++index)
            {
                if (animationTime < rotations_[index + 1].timeStamp)
                    return index;
            }
            return 0;
        }

        int GetScaleIndex(float animationTime)
        {
            for (int index = 0; index < num_scalings_ - 1; ++index)
            {
                if (animationTime < scales_[index + 1].timeStamp)
                    return index;
            }
            return 0;
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
            float scaleFactor = GetScaleFactor(positions_[p0Index].timeStamp,
                                               positions_[p1Index].timeStamp, animationTime);
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
            float scaleFactor = GetScaleFactor(rotations_[p0Index].timeStamp,
                                               rotations_[p1Index].timeStamp, animationTime);
            glm::quat finalRotation = glm::slerp(rotations_[p0Index].orientation, rotations_[p1Index].orientation, scaleFactor);
            finalRotation = glm::normalize(finalRotation);
            return glm::toMat4(finalRotation);
        }

        glm::mat4 InterpolateScaling(float animationTime)
        {
            if (1 == num_scalings_)
                return glm::scale(glm::mat4(1.0f), scales_[0].scale);

            int p0Index = GetScaleIndex(animationTime);
            int p1Index = p0Index + 1;
            float scaleFactor = GetScaleFactor(scales_[p0Index].timeStamp,
                                               scales_[p1Index].timeStamp, animationTime);
            glm::vec3 finalScale = glm::mix(scales_[p0Index].scale, scales_[p1Index].scale, scaleFactor);
            return glm::scale(glm::mat4(1.0f), finalScale);
        }

        void process_renumbering()
        {
            for (int i = 0; i < positions_.size(); i++)
            {
            }
        }

        std::vector<KeyPosition> positions_;
        std::vector<KeyRotation> rotations_;
        std::vector<KeyScale> scales_;

        int num_positions_;
        int num_rotations_;
        int num_scalings_;

        std::string name_;
        glm::mat4 local_transform_;

        // keyframe, position, rotation, scale
        std::map<float, int> time_positions_map_;
        std::map<float, int> time_rotations_map_;
        std::map<float, int> time_scales_map_;
    };

}

#endif