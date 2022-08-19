#include "entity.h"

namespace anim
{
    Entity::Entity(const std::string &name, int id, Entity *parent)
        : name_(name), id_(id), parent_(parent)
    {
    }
    void Entity::update()
    {
        if (is_deactivate_)
        {
            return;
        }
        world_ = local_; // transform_.get_mat4();
        if (parent_)
        {
            world_ = parent_->get_world_transformation() * world_;
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
    Entity *Entity::find(const std::string &name)
    {
        return find(root_, name);
    }
    Entity *Entity::find(Entity *entity, const std::string &name)
    {
        if (!entity || entity->name_ == name)
        {
            return entity;
        }
        Entity *ret = nullptr;
        for (auto &children : entity->children_)
        {
            ret = find(children.get(), name);
            if (ret)
            {
                return ret;
            }
        }
        return ret;
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