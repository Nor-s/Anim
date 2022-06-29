#include "model_property_layer.h"

#include "scene/scene.hpp"
#include "glcpp/model.h"
#include "glcpp/component/transform_component.h"
#include <imgui/imgui.h>

namespace ui
{
    ModelPropertyLayer::ModelPropertyLayer() = default;
    ModelPropertyLayer::~ModelPropertyLayer() = default;
    void ModelPropertyLayer::draw(glcpp::Model *model)
    {
        ImGui::Begin("Model Property");
        {
            if (ImGui::CollapsingHeader("Transform"))
            {
                if (model)
                {
                    draw_transform_slider(model->get_mutable_transform());
                }
                else
                {
                    ImGui::Text("No model selected");
                }
                ImGui::Separator();
            }
        }
        ImGui::End();
        ImGui::ShowDemoWindow();
    }
    void ModelPropertyLayer::draw_transform_slider(glcpp::TransformComponent &transform)
    {
        auto &rotation = transform.get_rotation();
        glm::vec3 r = rotation;
        ImGui::SliderFloat3("rotation", &r[0], 0.0f, 6.5f);
        transform.set_rotation({r.x, r.y, r.z});
        auto &scale = transform.get_scale();
        r = scale;
        ImGui::SliderFloat("scale", &r.x, 0.1f, 100.0f);
        transform.set_scale({r.x, r.x, r.x});
        auto &translation = transform.get_translation();
        r = translation;
        ImGui::SliderFloat3("translation", &r[0], -100.0f, 100.0f);
        transform.set_translation({r.x, r.y, r.z});
    }
}