#ifndef SRC_SCENE_SCENE_H
#define SRC_SCENE_SCENE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>
#include <memory>
#include "glcpp/component/component.hpp"

class SharedResources;

namespace glcpp
{
    class Framebuffer;
    class Camera;
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
    virtual glcpp::Entity *get_mutable_selected_entity() = 0;
    virtual std::shared_ptr<glcpp::Framebuffer> get_mutable_framebuffer()
    {
        return framebuffer_;
    }
    virtual std::shared_ptr<SharedResources> get_mutable_shared_resources()
    {
        return resources_;
    }
    virtual std::shared_ptr<SharedResources> &get_mutable_ref_shared_resources()
    {
        return resources_;
    }
    virtual std::shared_ptr<glcpp::Camera> &get_mutable_ref_camera()
    {
        return camera_;
    }
    virtual void set_size(uint32_t width, uint32_t height)
    {
        width_ = width;
        height_ = height;
    }
    virtual void set_delta_time(float dt)
    {
        delta_time_ = dt;
    }
    template <class T>
    T *get_component();
    template <class T>
    void add_component();

protected:
    std::vector<std::unique_ptr<glcpp::Component>> components_;
    std::shared_ptr<SharedResources> resources_;
    std::shared_ptr<glcpp::Framebuffer> framebuffer_;
    std::shared_ptr<glcpp::Camera> camera_;
    glm::mat4 projection_ = glm::mat4(1.0f);
    glm::mat4 view_ = glm::mat4(1.0f);
    float delta_time_ = 0.0f;
    uint32_t width_ = 800;
    uint32_t height_ = 600;
};

template <class T>
T *Scene::get_component()
{
    for (auto it = components_.begin(); it != components_.end(); ++it)
    {
        if ((*it)->get_type() == T::type)
            return static_cast<T *>((*it).get());
    }
    return nullptr;
}
template <class T>
void Scene::add_component()
{
    components_.push_back(std::make_unique<T>());
}

#endif