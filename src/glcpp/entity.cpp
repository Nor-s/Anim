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

    void Entity::set_model(std::shared_ptr<Model> model)
    {
        model_ = std::move(model);
    }

    void Entity::set_animation_component(std::shared_ptr<Animation> animation)
    {
        if (animation_component_)
        {
            animation_component_->set_animation(animation);
        }
        else
        {
            animation_component_ = std::make_unique<AnimationComponent>(animation);
        }
    }

    bool Entity::has_animation_component()
    {
        return animation_component_ != nullptr;
    }
}