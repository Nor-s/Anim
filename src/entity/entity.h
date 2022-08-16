#ifndef ANIM_ENTITY_ENTITY_H
#define ANIM_ENTITY_ENTITY_H

#include <vector>
#include <memory>
#include <string>

#include "components/component.h"
#include "components/transform_component.h"
namespace anim
{

    class Entity
    {
    public:
        Entity() = default;
        Entity(const std::string &name, int id, Entity *parent = nullptr);
        template <class T>
        T *get_component();
        template <class T>
        T *add_component();
        void update();
        void set_name(const std::string &name);
        void add_children(std::shared_ptr<Entity> &children);
        const std::string &get_name()
        {
            return name_;
        }
        const glm::mat4 get_local_transformation()
        {
            return local_transform_.get_mat4();
        }
        TransformComponent &get_mutable_local_transformation()
        {
            return local_transform_;
        }
        std::vector<std::shared_ptr<Entity>> &get_mutable_children()
        {
            return children_;
        }
        void set_world_transformation(const glm::mat4 &world)
        {
            world_ = world;
        }
        const glm::mat4 &get_world_transformation()
        {
            return world_;
        }

    private:
        std::string name_{};
        Entity *parent_ = nullptr;
        std::vector<std::shared_ptr<Component>> components_{};
        std::vector<std::shared_ptr<Entity>> children_{};
        TransformComponent local_transform_{};
        glm::mat4 world_{1.0f};
        int id_{-1};
    };

    template <class T>
    T *Entity::get_component()
    {
        for (auto &component : components_)
        {
            if (component->get_type() == T::type)
                return static_cast<T *>(component.get());
        }
        return nullptr;
    }
    template <class T>
    T *Entity::add_component()
    {
        auto cmp = get_component<T>();
        if (cmp)
        {
            return cmp;
        }
        components_.push_back(std::make_shared<T>());
        return static_cast<T *>(components_.back().get());
    }
}

#endif