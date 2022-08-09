#include "transform_component.h"
#include "utility.hpp"

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace glcpp
{
    glm::mat4 TransformComponent::get_mat4() const
    {
        glm::mat4 rotation = glm::toMat4(glm::quat(rotation_));

        return glm::translate(glm::mat4(1.0f), translation_) * rotation * glm::scale(glm::mat4(1.0f), scale_);
    }
    const glm::vec3 &TransformComponent::get_translation() const
    {
        return translation_;
    }
    const glm::vec3 &TransformComponent::get_rotation() const
    {
        return rotation_;
    }
    const glm::vec3 &TransformComponent::get_scale() const
    {
        return scale_;
    }
    TransformComponent &TransformComponent::set_translation(const glm::vec3 &vec)
    {
        translation_ = vec;
        return *this;
    }
    TransformComponent &TransformComponent::set_scale(const glm::vec3 &vec)
    {
        scale_ = vec;

        return *this;
    }
    TransformComponent &TransformComponent::set_scale(float scale)
    {
        set_scale(glm::vec3{scale, scale, scale});
        return *this;
    }

    TransformComponent &TransformComponent::set_rotation(const glm::vec3 &vec)
    {
        rotation_ = vec;

        return *this;
    }
    TransformComponent &TransformComponent::set_transform(const glm::mat4 &mat)
    {
        auto [t, r, s] = DecomposeTransform(mat);
        translation_ = t;
        rotation_ = glm::eulerAngles(r);
        scale_ = s;
        return *this;
    }

}