#ifndef ANIM_ENTITY_COMPONENT_ARMATURE_COMPONENT_H
#define ANIM_ENTITY_COMPONENT_ARMATURE_COMPONENT_H

#include "../component.h"
#include "../pose_component.h"
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>
#include "../../entity.h"

namespace anim
{
    class Mesh;
    class ArmatureComponent : public ComponentBase<ArmatureComponent>
    {
    public:
        void set_pose(PoseComponent *pose)
        {
            pose_ = pose;
        }
        void set_name(const std::string &name)
        {
            name_ = name;
        }
        void set_bone_id(int id)
        {
            id_ = id;
        }
        void set_bone_offset(const glm::mat4 &offset)
        {
            offset_ = offset;
        }
        PoseComponent *get_pose()
        {
            return pose_;
        }
        const glm::mat4 &get_bone_offset() const
        {
            return offset_;
        }
        void set_local_scale(int idx, float scale)
        {
            if (scale_.size() <= idx)
            {
                scale_.emplace_back(1.0f);
            }
            scale_[idx] = scale;
        }
        // http : // www.opengl-tutorial.org/kr/intermediate-tutorials/tutorial-17-quaternions/

        void set_local_rotation_with_target(int idx, const glm::vec3 &target)
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
                set_local_rotation(idx, glm::angleAxis(glm::radians(180.0f), axis));
            }
            else
            {
                float s = sqrt((1 + cos_theta) * 2);
                float invs = 1 / s;

                set_local_rotation(idx, glm::quat(
                                            s * 0.5f,
                                            axis.x * invs,
                                            axis.y * invs,
                                            axis.z * invs));
            }
        }
        void set_local_rotation(int idx, const glm::quat &quat)
        {
            if (rotation_.size() <= idx)
            {
                rotation_.emplace_back(glm::quat{});
            }
            rotation_[idx] = quat;
        }
        float get_local_scale()
        {
            return scale_.back();
        }
        int get_id()
        {
            return id_;
        }
        const glm::quat &get_local_rotation()
        {
            return rotation_.back();
        }
        void set_shader(Shader *shader)
        {
            shader_ = shader;
        }
        void set_entity(Entity *entity)
        {
            entity_ = entity;
        }
        void update()
        {
            // auto parent_world = entity->get_parent()->get_world_transform();
            // auto bind_pose = entity->get_local_transform();
            // auto animation_transform = pose_->get_current_transform(name_);

            // auto world = parent_world * bind_pose * animation_transform;

            // armature_->draw(world);

            // entity->set_world_transform();
        }

    private:
        PoseComponent *pose_;
        int id_{-1};
        std::string name_;
        std::vector<float> scale_;
        std::vector<glm::quat> rotation_{};
        glm::mat4 offset_{1.0f};
        static std::unique_ptr<Mesh> armature_;
        Entity *entity_;
        Shader *shader_;
    };
}

#endif