#ifndef ANIM_ENTITY_ENTITY_H
#define ANIM_ENTITY_ENTITY_H

#include <vector>
#include <memory>
#include <string>

#include "components/component.h"
namespace anim
{

    class Entity
    {
    public:
        Entity() = default;
        template <class T>
        T *get_component();
        template <class T>
        void add_component();
        void update();

    private:
        std::vector<std::unique_ptr<Component>> components_;
        std::vector<std::shared_ptr<Entity>> children_;
        Entity *parent_;
        std::string name_;
        uint32_t id_;
    };

    template <class T>
    T *Entity::get_component()
    {
        for (auto it = components_.begin(); it != components_.end(); ++it)
        {
            if ((*it)->get_type() == T::type)
                return static_cast<T *>((*it).get());
        }
        return nullptr;
    }
    template <class T>
    void Entity::add_component()
    {
        components_.push_back(std::make_unique<T>());
    }
}

#endif