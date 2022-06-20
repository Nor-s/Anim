#ifndef GLCPP_WORLD_TRANSFORM_COMPONENT_H
#define GLCPP_WORLD_TRANSFORM_COMPONENT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace glcpp
{
    class TransformComponent
    {
    public:
        glm::mat4 get_mat4() const;
        const glm::vec3& get_translation() const;
        const glm::vec3& get_rotation() const;
        const glm::vec3& get_scale() const;
        TransformComponent& set_translation(const glm::vec3& vec);
        TransformComponent& set_scale(const glm::vec3& vec);
        TransformComponent& set_rotation(const glm::vec3& vec);

    private:
        glm::vec3 translation_{0.0f, 0.0f, 0.0f};
        glm::vec3 scale_{1.f, 1.f, 1.f};
        glm::vec3 rotation_{0.0f, 0.0f, 0.0f};
    };

}

#endif