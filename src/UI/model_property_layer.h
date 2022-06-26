#ifndef UI_IMGUI_MODEL_PROPERTY_LAYER_H
#define UI_IMGUI_MODEL_PROPERTY_LAYER_H

namespace glcpp
{
    class Model;
    struct TransformComponent;
}

namespace ui
{
    class ModelPropertyLayer
    {
    public:
        void draw(glcpp::Model *model);

    private:
        void draw_transform_slider(glcpp::TransformComponent &transform);
    };
}

#endif
