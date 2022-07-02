#include "model_property_layer.h"

#include "scene/scene.hpp"
#include "scene/shared_resources.h"
#include "glcpp/entity.h"
#include "glcpp/anim/animation.hpp"
#include "glcpp/model.h"
#include "glcpp/component/transform_component.h"
#include <imgui/imgui.h>

namespace ui
{
    ModelPropertyLayer::ModelPropertyLayer() = default;

    ModelPropertyLayer::~ModelPropertyLayer() = default;

    void ModelPropertyLayer::draw(PropertiesContext &context, Scene *scene)
    {
        glcpp::Entity *entity = scene->get_mutable_selected_entity();
        SharedResources *resources = scene->get_mutable_ref_shared_resources().get();

        if (ImGui::Begin("Entity"))
        {
            if (ImGui::CollapsingHeader("Model&Animation"))
            {
                draw_model_list(context, resources, entity);
                ImGui::Separator();
            }
            if (ImGui::CollapsingHeader("Transform"))
            {
                if (entity)
                {
                    draw_transform_slider(entity->get_mutable_transform());
                    ImGui::Separator();
                    draw_transform_reset_button(entity->get_mutable_transform());
                }
                else
                {
                    ImGui::Text("No model selected");
                }
                ImGui::Separator();
            }
        }
        ImGui::End();
    }

    void ModelPropertyLayer::draw_model_list(PropertiesContext &context, const SharedResources *shared_resource, const glcpp::Entity *entity)
    {
        context.model_idx = entity->get_model_id();
        context.animation_idx = entity->get_animation_id();

        int &model_idx = context.model_idx;
        int &animation_idx = context.animation_idx;

        const auto &models = shared_resource->get_models();
        const auto &animations = shared_resource->get_animations();
        const char *names[] = {"Model", "Animation"};
        ImGuiStyle &style = ImGui::GetStyle();

        float child_w = (ImGui::GetContentRegionAvail().x - 1 * style.ItemSpacing.x) / 2;
        if (child_w < 1.0f)
            child_w = 1.0f;

        ImGui::PushID("##VerticalScrolling");
        for (int i = 0; i < 2; i++)
        {
            if (i > 0)
                ImGui::SameLine();
            ImGui::BeginGroup();

            const ImGuiWindowFlags child_flags = ImGuiWindowFlags_MenuBar;
            const ImGuiID child_id = ImGui::GetID((void *)(intptr_t)i);
            const bool child_is_visible = ImGui::BeginChild(child_id, ImVec2(child_w, 200.0f), true, child_flags);
            if (ImGui::BeginMenuBar())
            {
                ImGui::TextUnformatted(names[i]);
                ImGui::EndMenuBar();
            }
            if (child_is_visible)
            {
                if (i == 0)
                {
                    for (int idx = 0; idx < models.size(); idx++)
                    {
                        bool is_selected = (idx == model_idx);
                        if (ImGui::Selectable((std::to_string(idx) + ":" + models[idx]->get_name()).c_str(), is_selected))
                        {
                            model_idx = idx;
                        }
                    }
                }
                else if (entity->has_bone())
                {
                    for (int idx = 0; idx < animations.size(); idx++)
                    {
                        bool is_selected = (idx == animation_idx);
                        if (ImGui::Selectable((std::to_string(idx) + ":" + animations[idx]->get_name()).c_str(), is_selected))
                        {
                            animation_idx = idx;
                        }
                    }
                }
            }

            ImGui::EndChild();
            ImGui::EndGroup();
        }
        ImGui::PopID();
    }

    void ModelPropertyLayer::draw_transform_slider(glcpp::TransformComponent &transform)
    {
        auto &rotation = transform.get_rotation();
        glm::vec3 r = rotation;
        ImGui::Text("Rotate");
        ImGui::Text("x:");
        ImGui::SameLine();
        ImGui::DragFloat("##rotate.x", &r.x, 0.01f, -6.5f, 6.5f);
        ImGui::Text("y:");
        ImGui::SameLine();
        ImGui::DragFloat("##rotate.y", &r.y, 0.01f, -6.5f, 6.5f);
        ImGui::Text("z:");
        ImGui::SameLine();
        ImGui::DragFloat("##rotate.z", &r.z, 0.01f, -6.5f, 6.5f);
        transform.set_rotation({r.x, r.y, r.z});
        ImGui::Separator();

        auto &scale = transform.get_scale();
        r = scale;
        ImGui::Text("Scale");
        ImGui::Text("x:");
        ImGui::SameLine();
        ImGui::DragFloat("##scale", &r.x, 0.05f, 0.1f, 100.0f);
        transform.set_scale({r.x, r.x, r.x});
        ImGui::Separator();

        auto &translation = transform.get_translation();
        r = translation;
        ImGui::Text("Translate");
        ImGui::Text("x:");
        ImGui::SameLine();
        ImGui::DragFloat("##translation.x", &r.x, 1.0f, -1000.0f, 1000.0f);
        ImGui::Text("y:");
        ImGui::SameLine();
        ImGui::DragFloat("##translation.y", &r.y, 1.0f, -1000.0f, 1000.0f);
        ImGui::Text("z:");
        ImGui::SameLine();
        ImGui::DragFloat("##translation.z", &r.z, 1.0f, -1000.0f, 1000.0f);
        transform.set_translation({r.x, r.y, r.z});
    }
    void ModelPropertyLayer::draw_transform_reset_button(glcpp::TransformComponent &transform)
    {
        if (ImGui::Button("reset"))
        {
            transform.set_translation({0.0f, 0.0f, 0.0f}).set_rotation({0.0f, 0.0f, 0.0f}).set_scale({1.0f, 1.0f, 1.0f});
        }
    }
}