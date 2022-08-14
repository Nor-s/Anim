#include "entity.h"

namespace anim
{
    void Entity::update()
    {
        for (auto &component : components_)
        {
            component->update();
        }
        for (auto &child : children_)
        {
            child->update();
        }
    }
}