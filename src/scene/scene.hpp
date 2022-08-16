#ifndef SRC_SCENE_SCENE_H
#define SRC_SCENE_SCENE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>
#include <memory>
#include <entity/components/component.h>
#include <../resources/shared_resources.h>

namespace glcpp
{
    class Camera;
}
namespace anim
{
    class Framebuffer;
    class Entity;
}

class Scene
{
public:
    Scene() = default;
    virtual ~Scene() = default;
    virtual void init_framebuffer(uint32_t width, uint32_t height) = 0;
    virtual void pre_draw() = 0;
    virtual void draw() = 0;
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

protected:
    anim::Entity *selected_entity_{nullptr};
    std::shared_ptr<anim::SharedResources> resources_;
    std::shared_ptr<anim::Framebuffer> framebuffer_;
    std::shared_ptr<glcpp::Camera> camera_;
    float delta_time_ = 0.0f;
    uint32_t width_ = 800;
    uint32_t height_ = 600;
};

#endif