#include "model_property_layer.h"

#include "scene/scene.hpp"
#include "scene/shared_resources.h"
#include "glcpp/entity.h"
#include "glcpp/anim/animation.hpp"
#include "glcpp/model.h"
#include "glcpp/component/transform_component.h"
#include "imgui_helper.h"
#include <imgui/imgui.h>
#include "pixelate_framebuffer.h"

namespace ui
{
    ModelPropertyLayer::ModelPropertyLayer() = default;

    ModelPropertyLayer::~ModelPropertyLayer() = default;

    void ModelPropertyLayer::draw(PropertiesContext &context, Scene *scene)
    {
        glcpp::Entity *entity = scene->get_mutable_selected_entity();
        SharedResources *resources = scene->get_mutable_ref_shared_resources().get();

        if (ImGui::Begin("Properties"))
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
                    draw_transform_reset_button(entity->get_mutable_transform());
                    ImGui::Separator();
                    draw_transform_slider(entity->get_mutable_transform());
                }
                else
                {
                    ImGui::Text("No model selected");
                }
                ImGui::Separator();
            }
            if (auto pixelate_component = scene->get_component<PixelateStateComponent>(); pixelate_component)
            {
                draw_pixelate_properties(pixelate_component);
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
                        std::string name = std::to_string(idx) + ":" + models[idx]->get_name();
                        if (ImGui::Selectable(name.c_str(), is_selected))
                        {
                            model_idx = idx;
                        }
                    }
                }
                else if (entity->has_bone())
                {
                    for (int idx = 0; idx < animations.size(); idx++)
                    {
                        std::string name = std::to_string(idx) + ":" + animations[idx]->get_name();

                        bool is_selected = (idx == animation_idx);
                        if (ImGui::Selectable(name.c_str(), is_selected))
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
        auto vec = transform.get_rotation();
        bool result = DragPropertyXYZ("Rotation", vec);
        if (result)
        {
            transform.set_rotation(vec);
        }

        vec = transform.get_scale();
        result = DragPropertyXYZ("Scale", vec);
        if (result)
        {
            transform.set_scale(vec);
        }

        vec = transform.get_translation();
        result = DragPropertyXYZ("Translation", vec);
        if (result)
        {
            transform.set_translation(vec);
        }
    }
    void ModelPropertyLayer::draw_transform_reset_button(glcpp::TransformComponent &transform)
    {
        if (ImGui::Button("reset"))
        {
            transform.set_translation({0.0f, 0.0f, 0.0f}).set_rotation({0.0f, 0.0f, 0.0f}).set_scale({1.0f, 1.0f, 1.0f});
        }
    }
    void ModelPropertyLayer::draw_pixelate_properties(PixelateStateComponent *state)
    {
        if (ImGui::CollapsingHeader("Outline"))
        {
            ImGui::Text("enable:");
            ImGui::SameLine();
            ImGui::Checkbox("##outline", state->get_mutable_pointer_is_outline());
            ImGui::Text("color:");
            ImGui::SameLine();
            ImGui::ColorEdit3("##outcolor", &(*state->get_mutable_pointer_outline_color())[0]);
            ImGui::Separator();
        }
        if (ImGui::CollapsingHeader("Pixelate"))
        {
            ImGui::Text("mod:");
            ImGui::SameLine();
            int factor = static_cast<int>(state->get_factor());
            ImGui::SliderInt("##mod", &factor, 1, 100);
            (*state->get_mutable_pointer_factor()) = factor;
            ImGui::Separator();
        }
    }
}