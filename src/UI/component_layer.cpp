#include "component_layer.h"

#include "scene/scene.hpp"
#include "imgui_helper.h"
#include <imgui/imgui.h>
#include <entity/entity.h>
#include <entity/components/animation_component.h>
#include <animation/animation.h>

using namespace anim;

namespace ui
{
    ComponentLayer::ComponentLayer() = default;

    ComponentLayer::~ComponentLayer() = default;

    void ComponentLayer::draw(ComponentContext &context, Scene *scene)
    {
        Entity *entity = scene->get_mutable_selected_entity();
        SharedResources *resources = scene->get_mutable_ref_shared_resources().get();

        if (ImGui::Begin("Component"))
        {
            if (entity)
            {
                if (ImGui::CollapsingHeader("Transform"))
                {
                    draw_transform(entity);
                    ImGui::Separator();
                }
                if (auto root = entity->get_mutable_root(); root)
                {
                    if (auto animation = root->get_component<AnimationComponent>(); animation && ImGui::CollapsingHeader("Animation"))
                    {
                        draw_animation(context, resources, root, animation);
                        ImGui::Separator();
                    }
                }
            }
        }
        ImGui::End();
    }

    void ComponentLayer::draw_animation(ComponentContext &context, const SharedResources *shared_resource, const Entity *entity, const AnimationComponent *animation)
    {
        context.current_animation_idx = animation->get_animation()->get_id();
        int animation_idx = context.current_animation_idx;

        const auto &animations = shared_resource->get_animations();
        const char *names[] = {"Animation"};
        ImGuiStyle &style = ImGui::GetStyle();

        float child_w = (ImGui::GetContentRegionAvail().x - 1 * style.ItemSpacing.x);
        if (child_w < 1.0f)
            child_w = 1.0f;

        ImGui::PushID("##VerticalScrolling");
        for (int i = 0; i < 1; i++)
        {
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

            ImGui::EndChild();
        }
        auto animc = const_cast<AnimationComponent *>(animation);
        auto anim = animc->get_mutable_animation();
        ImGui::Text(("duration: " + std::to_string(anim->get_duration())).c_str());
        ImGui::Text(("fps: " + std::to_string(anim->get_fps())).c_str());
        float &fps = animc->get_mutable_custom_tick_per_second();
        if (animation_idx != context.current_animation_idx)
        {
            context.new_animation_idx = animation_idx;
            context.is_changed_animation = true;
        }
        ImGui::DragFloat("custom fps", &fps, 1.0f, 1.0f, 144.0f);
        ImGui::PopID();
    }

    void ComponentLayer::draw_transform(anim::Entity *entity)
    {
        auto &world = entity->get_world_transformation();
        auto &local = entity->get_local();
        TransformComponent transform;
        transform.set_transform(world);
        ImGui::Text("World");
        DragPropertyXYZ("Translation", transform.translation_);
        DragPropertyXYZ("Rotation", transform.rotation_);
        DragPropertyXYZ("Scale", transform.scale_);

        ImGui::Separator();

        transform.set_transform(local);
        ImGui::Text("Local");
        DragPropertyXYZ("Translation", transform.translation_);
        DragPropertyXYZ("Rotation", transform.rotation_);
        DragPropertyXYZ("Scale", transform.scale_);
    }
    void ComponentLayer::draw_transform_reset_button(anim::TransformComponent &transform)
    {
        if (ImGui::Button("reset"))
        {
            transform.set_translation({0.0f, 0.0f, 0.0f}).set_rotation({0.0f, 0.0f, 0.0f}).set_scale({1.0f, 1.0f, 1.0f});
        }
    }
}