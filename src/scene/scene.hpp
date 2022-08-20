#ifndef SRC_SCENE_SCENE_H
#define SRC_SCENE_SCENE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>
#include <memory>
#include <entity/components/component.h>
#include <../resources/shared_resources.h>
#include <../entity/entity.h>
#include <../entity/components/pose_component.h>
#include <../entity/components/renderable/armature_component.h>

namespace glcpp
{
    class Camera;
}
namespace anim
{
    class Framebuffer;
}

class Scene
{
public:
    Scene() = default;
    virtual ~Scene() = default;
    virtual void init_framebuffer(uint32_t width, uint32_t height) = 0;
    virtual void pre_draw() = 0;
    virtual void draw() = 0;
    virtual void picking(int x, int y, bool is_only_bone) = 0;
    virtual anim::Entity *get_mutable_selected_entity()
    {
        return selected_entity_;
    }
    virtual std::shared_ptr<anim::Framebuffer> get_mutable_framebuffer()
    {
        return framebuffer_;
    }
    virtual std::shared_ptr<anim::SharedResources> get_mutable_shared_resources()
    {
        return resources_;
    }
    virtual std::shared_ptr<anim::SharedResources> &get_mutable_ref_shared_resources()
    {
        return resources_;
    }
    virtual std::shared_ptr<glcpp::Camera> &get_mutable_ref_camera()
    {
        return camera_;
    }
    virtual void set_size(uint32_t width, uint32_t height)
    {
        if (width > 0)
            width_ = width;
        if (height > 0)
            height_ = height;
    }
    virtual void set_delta_time(float dt)
    {
        delta_time_ = dt;
        resources_->set_dt(delta_time_);
    }
    void set_selected_entity(anim::Entity *entity)
    {
        if (selected_entity_)
        {
            selected_entity_->set_is_selected(false);
        }
        if (entity)
        {
            entity->set_is_selected(true);
            auto parent = entity->get_mutable_parent();
            if (parent)
            {
                auto &child = parent->get_mutable_children();
                int idx = 0;
                for (int i = 0; i < child.size(); i++)
                {
                    if (child[i]->get_id() == entity->get_id())
                    {
                        idx = i;
                        break;
                    }
                }
                std::swap(child[idx], child.back());
            }
            auto root = entity->get_mutable_root();
            if (root && entity->get_component<anim::ArmatureComponent>())
            {
                int idx = 0;
                auto &child = root->get_mutable_children();
                for (int i = 0; i < child.size(); i++)
                {
                    if (child[i]->get_component<anim::ArmatureComponent>())
                    {
                        idx = i;
                        break;
                    }
                }
                std::swap(child[idx], child.back());
            }
        }

        selected_entity_ = entity;
    }
    void set_selected_entity(int id)
    {
        set_selected_entity(resources_->get_entity(id));
    }

protected:
    anim::Entity *selected_entity_{nullptr};
    std::shared_ptr<anim::SharedResources> resources_;
    std::shared_ptr<anim::Framebuffer> framebuffer_;
    std::shared_ptr<anim::Framebuffer> offscreen_framebuffer_;
    std::shared_ptr<glcpp::Camera> camera_;
    float delta_time_ = 0.0f;
    uint32_t width_ = 800;
    uint32_t height_ = 600;
};

#endif