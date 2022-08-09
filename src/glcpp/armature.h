#ifndef GLCPP_ARMATURE_H
#define GLCPP_ARMATURE_H

#include "component/OBB_component.h"

namespace glcpp
{
    class Shader;
    class Armature
    {
    public:
        Armature(int id);
        ~Armature();
        void draw(Shader &shader, const glm::mat4 &view, const glm::mat4 &proj, const glm::mat4 &world = glm::mat4(1.0f));
        TransformComponent &get_mutable_transform();
        float get_scale();
        void set_scale(float scale);

    private:
        int id_;
        TransformComponent bind_pose_transform_{};
        OBBComponent OBB_{};
        uint32_t VAO_{};
        uint32_t VBO_{};
        uint32_t normal_VBO_{};
        uint32_t bone_id_VBO_{};
        uint32_t bone_weight_VBO_{};
        float scale_{1.0};
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