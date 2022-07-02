#ifndef GLCPP_ENTITY_H
#define GLCPP_ENTITY_H

#include "component/transform_component.h"
#include <glm/glm.hpp>
#include <memory>

namespace glcpp
{
    class Model;
    class Shader;
    class Animation;
    class AnimationComponent;

    class Entity
    {
    public:
        Entity();
        ~Entity();

        void draw(Shader &shader, const glm::mat4 &view, const glm::mat4 &projection);

        Model *get_mutable_model();
        TransformComponent &get_mutable_transform();
        AnimationComponent *get_mutable_pointer_animation_component();
        const AnimationComponent *get_pointer_animation_component() const;
        int get_model_id() const;
        int get_animation_id() const;

        void set_model(std::shared_ptr<Model> model, int id);
        void set_animation_component(std::shared_ptr<Animation> animation, int id);

        bool has_animation_component() const;

        bool has_bone() const;

    private:
        std::shared_ptr<Model> model_;
        TransformComponent transform_component_{};
        std::unique_ptr<AnimationComponent> animation_component_;
        int model_id_{-1};
        int animation_id_{-1};
    };
}

#endif