#ifndef GLCPP_WORLD_TRANSFORM_COMPONENT_H
#define GLCPP_WORLD_TRANSFORM_COMPONENT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace glcpp
{
    class WorldTransformComponent
    {
    public:
        glm::mat4 get_mat4()
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
                    {translation_.x, translation_.y, translation_.z, 1.0f}};
        }
        const glm::vec3 &get_translation() const
        {
            return translation_;
        }
        const glm::vec3 &get_rotation() const
        {
            return rotation_;
        }
        const glm::vec3 &get_scale() const
        {
            return scale_;
        }
        WorldTransformComponent &set_translation(const glm::vec3 &vec)
        {
            translation_ = vec;
            return *this;
        }
        WorldTransformComponent &set_scale(const glm::vec3 &vec)
        {
            scale_ = vec;

            return *this;
        }
        WorldTransformComponent &set_rotation(const glm::vec3 &vec)
        {
            rotation_ = vec;

            return *this;
        }

    private:
        glm::vec3 translation_{0.0f, 0.0f, 0.0f};
        glm::vec3 scale_{1.f, 1.f, 1.f};
        glm::vec3 rotation_{0.0f, 0.0f, 0.0f};
    };

}

#endif