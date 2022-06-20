#include "transform_component.h"

namespace glcpp {
    glm::mat4 TransformComponent::get_mat4() const
    {

        const float c3 = glm::cos(rotation_.z);
        const float s3 = glm::sin(rotation_.z);
        const float c2 = glm::cos(rotation_.x);
        const float s2 = glm::sin(rotation_.x);
        const float c1 = glm::cos(rotation_.y);
        const float s1 = glm::sin(rotation_.y);
        return

        {

            {
                scale_.x * (c1 * c3 + s1 * s2 * s3),
                scale_.x * (c2 * s3),
                scale_.x * (c1 * s2 * s3 - c3 * s1),
                0.0f,
            },
            {
                scale_.y * (c3 * s1 * s2 - c1 * s3),
                scale_.y * (c2 * c3),
                scale_.y * (c1 * c3 * s2 + s1 * s3),
                0.0f,
            },
            {
                scale_.z * (c2 * s1),
                scale_.z * (-s2),
                scale_.z * (c1 * c2),
                0.0f,
            },
            {translation_.x, translation_.y, translation_.z, 1.0f} };
    }
    const glm::vec3& TransformComponent::get_translation() const
    {
        return translation_;
    }
    const glm::vec3& TransformComponent::get_rotation() const
    {
        return rotation_;
    }
    const glm::vec3& TransformComponent::get_scale() const
    {
        return scale_;
    }
    TransformComponent& TransformComponent::set_translation(const glm::vec3& vec)
    {
        translation_ = vec;
        return *this;
    }
    TransformComponent& TransformComponent::set_scale(const glm::vec3& vec)
    {
        scale_ = vec;

        return *this;
    }
    TransformComponent& TransformComponent::set_rotation(const glm::vec3& vec)
    {
        rotation_ = vec;

        return *this;
    }
}