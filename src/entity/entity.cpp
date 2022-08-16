#include "entity.h"

namespace anim
{
    Entity::Entity(const std::string &name, int id, Entity *parent)
        : name_(name), id_(id), parent_(parent)
    {
    }
    void Entity::update()
    {
        if (parent_)
        {
            world_ = parent_->world_ * local_transform_.get_mat4();
        }
        for (auto &component : components_)
        {
            component->update();
        }
        for (auto &child : children_)
        {
            child->update();
        }
    }
    void Entity::set_name(const std::string &name)
    {
        name_ = name;
    }
    void Entity::add_children(std::shared_ptr<Entity> &children)
    {
        if (children->id_ != this->id_)
        {
            children->parent_ = this;
            children_.push_back(children);
        }
    }

}