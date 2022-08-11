#include "armature.h"

#include <glad/glad.h>
#include "shader.h"
#include <array>
#include <iostream>

namespace glcpp
{
    Armature::Armature(int id)
        : id_(id)
    {
        glGenVertexArrays(1, &VAO_);
        glBindVertexArray(VAO_);
        glGenBuffers(1, &VBO_);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_), &vertices_[0], GL_STATIC_DRAW);

        // position attribute
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

        glGenBuffers(1, &normal_VBO_);
        glBindBuffer(GL_ARRAY_BUFFER, normal_VBO_);
        float normal[72]{0.0f};
        for (int i = 0; i < 72; i += 9)
        {
            glm::vec3 a{vertices_[i], vertices_[i + 1], vertices_[i + 2]};
            glm::vec3 b{vertices_[i + 3], vertices_[i + 4], vertices_[i + 5]};
            glm::vec3 c{vertices_[i + 6], vertices_[i + 7], vertices_[i + 8]};
            glm::vec3 normal_v = glm::normalize(glm::cross(b - a, c - a));
            normal[i] = normal_v.x;
            normal[i + 1] = normal_v.y;
            normal[i + 2] = normal_v.z;
            normal[i + 3] = normal_v.x;
            normal[i + 4] = normal_v.y;
            normal[i + 5] = normal_v.z;
            normal[i + 6] = normal_v.x;
            normal[i + 7] = normal_v.y;
            normal[i + 8] = normal_v.z;
        }
        glEnableVertexAttribArray(1);
        glBufferData(GL_ARRAY_BUFFER, sizeof(normal), &normal, GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

        glGenBuffers(1, &bone_id_VBO_);
        glBindBuffer(GL_ARRAY_BUFFER, bone_id_VBO_);
        int size = 24 * 4;
        int bone_id[24 * 4];
        for (int i = 0; i < size; i += 4)
        {
            bone_id[i] = id_;
            bone_id[i + 1] = id_;
            bone_id[i + 2] = id_;
            bone_id[i + 3] = id_;
        }
        glEnableVertexAttribArray(5);
        glBufferData(GL_ARRAY_BUFFER, sizeof(int) * size, bone_id, GL_STATIC_DRAW);
        glVertexAttribIPointer(5, 4, GL_INT, 4 * sizeof(int), (void *)0);

        glGenBuffers(1, &bone_weight_VBO_);
        glBindBuffer(GL_ARRAY_BUFFER, bone_weight_VBO_);
        float bone_weight[24 * 4];
        for (int i = 0; i < size; i += 4)
        {
            bone_weight[i] = 1.0f;
            bone_weight[i + 1] = .0f;
            bone_weight[i + 2] = .0f;
            bone_weight[i + 3] = .0f;
        }
        glEnableVertexAttribArray(6);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * size, &bone_weight[0], GL_STATIC_DRAW);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        scale_.push_back(1.0f);
        rotation_.emplace_back(glm::quat{});
    }
    Armature::~Armature()
    {
        glDeleteVertexArrays(1, &VAO_);
        glDeleteBuffers(1, &VBO_);
        glDeleteBuffers(1, &normal_VBO_);
        glDeleteBuffers(1, &bone_id_VBO_);
        glDeleteBuffers(1, &bone_weight_VBO_);
    }
    void Armature::draw(Shader &shader, const glm::mat4 &view, const glm::mat4 &proj, const glm::mat4 &world, float depth)
    {
        shader.use();
        shader.set_mat4("projection", proj);
        shader.set_mat4("view", view);

        glm::vec3 diffuse = glm::mix(glm::vec3(0.1f, 0.1f, 1.0f), glm::vec3(0.2f, 0.4f, 0.6f), depth);
        if (depth < 0.6f)
        {
            diffuse = glm::mix(glm::vec3(0.6f, 0.2f, 0.6f), diffuse, depth / 0.6f);
        }

        shader.set_vec3("material.ambient", {0.0f, 0.0f, 0.0f});
        shader.set_vec3("material.diffuse", diffuse);
        shader.set_vec3("material.specular", 0.3f);
        shader.set_float("material.shininess", 0.3f);
        shader.set_bool("material.has_diffuse_texture", false);
        shader.set_vec3("dir_lights[0].direction", 0.0f, -1.0f, -1.0f);
        shader.set_vec3("dir_lights[0].ambient", 0.5f);
        shader.set_vec3("dir_lights[0].diffuse", 0.8f);
        shader.set_vec3("dir_lights[0].specular", 0.4f);

        shader.set_mat4("model", world);
        glBindVertexArray(VAO_);

        for (int i = 0; i < rotation_.size(); i++)
        {
            glm::mat4 r = glm::mat4(rotation_[i]);
            glm::mat4 s = glm::scale(glm::mat4(1.0f), glm::vec3(scale_[i]));
            shader.set_mat4("armature", bind_pose_transform_.get_mat4() * r * s);
            glDrawArrays(GL_TRIANGLES, 0, 24);
        }
        glBindVertexArray(0);
    }
    TransformComponent &Armature::get_mutable_transform()
    {
        return bind_pose_transform_;
    }
    float Armature::get_scale()
    {
        return scale_.back();
    }
    const glm::quat &Armature::get_quat() const
    {
        return rotation_.back();
    }
    void Armature::set_scale(int idx, float scale)
    {
        if (scale_.size() <= idx)
        {
            scale_.emplace_back(1.0f);
        }
        scale_[idx] = scale;
    }
    // http : // www.opengl-tutorial.org/kr/intermediate-tutorials/tutorial-17-quaternions/
    void Armature::set_rotation_with_target(int idx, const glm::vec3 &target)
    {
        glm::vec3 origin = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f));
        glm::vec3 t = glm::normalize(target);
        float cos_theta = glm::dot(origin, t);
        glm::vec3 axis = glm::cross(origin, t);
        if (cos_theta < -1 + 0.001f)
        {
            axis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), origin);
            if (glm::length2(axis) < 0.01f)
            {
                axis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), origin);
            }
            axis = glm::normalize(axis);
            set_rotation(idx, glm::angleAxis(glm::radians(180.0f), axis));
        }
        else
        {
            float s = sqrt((1 + cos_theta) * 2);
            float invs = 1 / s;

            set_rotation(idx, glm::quat(
                                  s * 0.5f,
                                  axis.x * invs,
                                  axis.y * invs,
                                  axis.z * invs));
        }
    }
    void Armature::set_rotation(int idx, const glm::quat &quat)
    {
        if (rotation_.size() <= idx)
        {
            rotation_.emplace_back(glm::quat{});
        }
        rotation_[idx] = quat;
    }
}