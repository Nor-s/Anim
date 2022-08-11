#ifndef GLCPP_ARMATURE_H
#define GLCPP_ARMATURE_H

#define GLM_FORCE_RADIANS
#include "component/OBB_component.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <vector>
namespace glcpp
{
    class Shader;
    class Armature
    {
    public:
        Armature(int id);
        ~Armature();
        void draw(Shader &shader, const glm::mat4 &view, const glm::mat4 &proj, const glm::mat4 &world, float depth = 0.0f);
        TransformComponent &get_mutable_transform();
        float get_scale();
        const glm::quat &get_quat() const;
        void set_scale(int idx, float scale);
        void set_rotation_with_target(int idx, const glm::vec3 &target);
        void set_rotation(int idx, const glm::quat &quat);

    private:
        int id_;
        std::vector<float> scale_;
        std::vector<glm::quat> rotation_{};
        TransformComponent bind_pose_transform_{};
        OBBComponent OBB_{};
        uint32_t VAO_{};
        uint32_t VBO_{};
        uint32_t normal_VBO_{};
        uint32_t bone_id_VBO_{};
        uint32_t bone_weight_VBO_{};
        inline static const float vertices_[24 * 3] = {
            // positions
            // front bottom
            -0.12f, 0.2f, 0.12f,
            0.0f, 0.0f, 0.0f,
            0.12f, 0.2f, 0.12f,
            // right
            0.12f, 0.2f, 0.12f,
            0.0f, 0.0f, 0.0f,
            0.12f, 0.2f, -0.12f,
            // left
            -0.12f, 0.2f, -0.12f,
            0.0f, 0.0f, 0.0f,
            -0.12f, 0.2f, 0.12f,
            // back
            0.12f, 0.2f, -0.12f,
            0.0f, 0.0f, 0.0f,
            -0.12f, 0.2f, -0.12f,

            // front top

            0.12f, 0.2f, 0.12f,
            0.0f, 1.0f, 0.0f,
            -0.12f, 0.2f, 0.12f,
            // right
            0.12f, 0.2f, -0.12f,
            0.0f, 1.0f, 0.0f,
            0.12f, 0.2f, 0.12f,
            // left
            -0.12f, 0.2f, 0.12f,
            0.0f, 1.0f, 0.0f,
            -0.12f, 0.2f, -0.12f,
            // back
            -0.12f, 0.2f, -0.12f,
            0.0f, 1.0f, 0.0f,
            0.12f, 0.2f, -0.12f};
    };
}

#endif