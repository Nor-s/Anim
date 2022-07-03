#include "entity.h"

#include "component/animation_component.h"
#include "shader.h"
#include "model.h"

namespace glcpp
{
    Entity::Entity() = default;
    Entity::~Entity() = default;
    void Entity::draw(Shader &shader, const glm::mat4 &view, const glm::mat4 &projection)
    {
        if (model_)
        {
            model_->draw(shader, view, projection, transform_component_.get_mat4());
        }
    }
    Model *Entity::get_mutable_model()
    {
        return model_.get();
    }

    TransformComponent &Entity::get_mutable_transform()
    {
        return transform_component_;
    }

    AnimationComponent *Entity::get_mutable_pointer_animation_component()
    {
        return animation_component_.get();
    }

    const AnimationComponent *Entity::get_pointer_animation_component() const
    {
        return animation_component_.get();
    }

    int Entity::get_model_id() const
    {
        return model_id_;
    }

    int Entity::get_animation_id() const
    {
        return animation_id_;
    }

    void Entity::set_model(std::shared_ptr<Model> model, int id)
    {
#ifndef NDEBUG
        std::cout << "SET ID: " << id << std::endl;
#endif
        model_ = std::move(model);
        model_id_ = id;
    }

    void Entity::set_animation_component(std::shared_ptr<Animation> animation, int id)
    {
        if (animation_component_)
        {
            animation_component_->set_animation(std::move(animation));
        }
        else
        {
            animation_component_ = std::make_unique<AnimationComponent>(std::move(animation));
        }
        animation_id_ = id;
    }

    bool Entity::has_animation_component() const
    {
        return animation_component_ != nullptr;
    }
    bool Entity::has_bone() const
    {
        if (model_)
        {
            return (model_->get_mutable_bone_info_map().size() != 0);
        }
        else
        {
            return false;
        }
    }
}