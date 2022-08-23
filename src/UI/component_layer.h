#ifndef UI_IMGUI_COMPONENT_LAYER_H
#define UI_IMGUI_COMPONENT_LAYER_H

#include "ui_context.h"

class Scene;
namespace anim
{
    class SharedResources;
    class Entity;
    class TransformComponent;
    class AnimationComponent;
}

namespace ui
{
    class ComponentLayer
    {
    public:
        ComponentLayer();
        ~ComponentLayer();
        void draw(ComponentContext &context, Scene *scene);

    private:
        void draw_animation(ComponentContext &context, const anim::SharedResources *shared_resource, const anim::Entity *entity, const anim::AnimationComponent *animation);
        void draw_transform(anim::Entity *entity);
        void draw_transform_reset_button(anim::TransformComponent &transform);
    };
}

#endif
