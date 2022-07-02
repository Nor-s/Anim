#ifndef UI_IMGUI_MODEL_PROPERTY_LAYER_H
#define UI_IMGUI_MODEL_PROPERTY_LAYER_H

class Scene;
class SharedResources;
namespace glcpp
{
    class Entity;
    struct TransformComponent;
}

namespace ui
{
    struct PropertiesContext
    {
        int model_idx{-1};
        int animation_idx{-1};
    };
    class ModelPropertyLayer
    {
    public:
        ModelPropertyLayer();
        ~ModelPropertyLayer();
        void draw(PropertiesContext &context, Scene *scene);

    private:
        void draw_model_list(PropertiesContext &context, const SharedResources *shared_resource, const glcpp::Entity *entity);
        void draw_transform_slider(glcpp::TransformComponent &transform);
        void draw_transform_reset_button(glcpp::TransformComponent &transform);
    };
}

#endif
