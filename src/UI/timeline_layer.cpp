#include "timeline_layer.h"
#include "text_edit_layer.h"
#include "scene/scene.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_neo_sequencer.h>
#include <imgui/icons/icons.h>
#include <imgui/ImGuizmo.h>

#include <entity/entity.h>
#include <resources/shared_resources.h>
#include <animation/animation.h>
#include <animation/animator.h>
#include <animation/bone.h>
#include <entity/components/pose_component.h>
#include <entity/components/animation_component.h>
#include <glm/gtc/type_ptr.hpp>

#include "imgui_helper.h"

#include <iostream>

using namespace anim;

namespace ui
{

    TimelineLayer::TimelineLayer()
    {
        text_editor_.reset(new TextEditLayer());
        text_editor_->init();
    }

    void TimelineLayer::draw(Scene *scene, UiContext &ui_context)
    {
        init_context(ui_context, scene);
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize;

        if (is_hovered_zoom_slider_)
        {
            window_flags |= ImGuiWindowFlags_NoScrollWithMouse;
        }

        ImGui::Begin("Animation", 0, window_flags);
        {
            draw_animator_status(ui_context);
            ImGui::BeginChild("##Timeline", ImVec2(0, 0), false, window_flags);
            {
                draw_sequencer(ui_context);
            }
            ImGui::EndChild();
        }
        ImGui::End();
    }
    inline void TimelineLayer::init_context(UiContext &ui_context, Scene *scene)
    {
        auto &context = ui_context.timeline;
        scene_ = scene;
        entity_ = scene->get_mutable_selected_entity();
        root_entity_ = nullptr;
        if (entity_)
        {
            root_entity_ = entity_->get_mutable_root();
        }
        resources_ = scene->get_mutable_shared_resources().get();
        animator_ = resources_->get_mutable_animator();
        context.fps = animator_->get_fps();
        context.start_frame = static_cast<int>(animator_->get_start_time());
        context.end_frame = static_cast<int>(animator_->get_end_time());
        context.current_frame = static_cast<int>(animator_->get_current_time());
        context.is_recording = animator_->get_is_recording();
        context.is_stop = animator_->get_is_stop();
        if (!context.is_stop)
        {
            context.is_forward = (animator_->get_direction() > 0.0f) ? true : false;
            context.is_backward = !context.is_forward;
        }
    }

    void TimelineLayer::draw_animator_status(UiContext &ui_context)
    {
        auto &context = ui_context.timeline;
        ImGuiIO &io = ImGui::GetIO();

        ImVec2 button_size(40.0f, 0.0f);
        ImVec2 small_button_size(32.0f, 0.0f);
        const float item_spacing = ImGui::GetStyle().ItemSpacing.x;
        float width = ImGui::GetContentRegionAvail().x;

        ImGui::PushItemWidth(30.0f);
        DragFloatProperty(ICON_MD_SPEED, context.fps, 1.0f, 1.0f, 300.0f);

        ImGui::SameLine();

        auto current_cursor = ImGui::GetCursorPosX();
        auto next_pos = ImGui::GetWindowWidth() / 2.0f - button_size.x - small_button_size.x - item_spacing;
        if (next_pos < current_cursor)
        {
            next_pos = current_cursor;
        }
        ImGui::SameLine(next_pos);

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0f);
        ImGui::PushStyleColor(ImGuiCol_Text, {1.0f, 0.3f, 0.2f, 0.8f});
        {
            ToggleButton(ICON_KI_REC, &context.is_recording, small_button_size);
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Text, {1.0f, 1.0f, 1.0f, 1.0f});
            ToggleButton(ICON_KI_CARET_LEFT, &context.is_backward, small_button_size, &context.is_clicked_play_back);
            ImGui::SameLine();
            ToggleButton(ICON_KI_CARET_RIGHT, &context.is_forward, small_button_size, &context.is_clicked_play);
            ImGui::SameLine();
            ToggleButton(ICON_KI_PAUSE, &context.is_stop, small_button_size);
            ImGui::SameLine();
        }
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();

        current_cursor = ImGui::GetCursorPosX();
        auto font_size = ImGui::CalcTextSize("Current Start End").x;
        next_pos = width - font_size + (-30.0f - item_spacing) * 3;

        if (next_pos < current_cursor)
        {
            next_pos = current_cursor;
        }
        ImGui::SameLine(next_pos);
        context.is_current_frame_changed = DragIntProperty("Current", context.current_frame, 1.0f, context.start_frame, context.end_frame);
        ImGui::SameLine();
        DragIntProperty("Start", context.start_frame, 1.0f, 0, context.end_frame - 1);
        ImGui::SameLine();
        DragIntProperty("End", context.end_frame, 1.0f, context.start_frame + 1, 10000);
        ImGui::PopItemWidth();
    }

    void TimelineLayer::draw_sequencer(UiContext &ui_context)
    {
        auto &context = ui_context.timeline;
        uint32_t current = static_cast<uint32_t>(animator_->get_current_time());
        uint32_t start = static_cast<uint32_t>(animator_->get_start_time());
        uint32_t end = static_cast<uint32_t>(animator_->get_end_time());
        uint32_t before = current;
        // TODO: begin neo sequencer : uint32_t to int
        if (ImGui::BeginNeoSequencer("Sequencer", &current, &start, &end))
        {
            if (root_entity_ && root_entity_->get_component<AnimationComponent>())
            {
                auto anim_component = root_entity_->get_component<AnimationComponent>();
                if (anim_component && anim_component->get_animation())
                {
                    draw_keyframes(ui_context, anim_component->get_animation());
                    // draw_keyframe_popup(ui_context);
                }
            }
            is_hovered_zoom_slider_ = false;
            if (ImGui::IsZoomSliderHovered())
            {
                is_hovered_zoom_slider_ = true;
            }
            ImGui::EndNeoSequencer();
        }

        // update current time
        if (before != current)
        {
            context.is_current_frame_changed = true;
            context.current_frame = static_cast<int>(current);
        }
    }

    void TimelineLayer::draw_keyframes(UiContext &ui_context, const Animation *animation)
    {
        auto &context = ui_context.timeline;
        auto &entity_context = ui_context.entity;
        if (ImGui::BeginNeoGroup("Transform", &is_opened_transform_))
        {
            auto &name_bone_map = animation->get_name_bone_map();
            bool is_hovered = false;
            for (auto &bone : name_bone_map)
            {
                float factor = bone.second->get_factor();
                auto &keys = bone.second->get_time_set();
                const char *name = bone.second->get_name().c_str();

                if (ImGui::BeginNeoTimeline(name))
                {
                    for (auto key : keys)
                    {
                        uint32_t ukey = static_cast<uint32_t>(floorf(key * factor));
                        if (ImGui::Keyframe(&ukey, &is_hovered) && is_hovered && ImGui::IsItemClicked())
                        {
                            // clicked_frame_ = ukey;
                            // clicked_time_ = key;
                            // clicked_bone_ = bone.second.get();
                            auto selected_entity = root_entity_->find(bone.second->get_name());
                            if (selected_entity)
                            {
                                entity_context.is_changed_selected_entity = true;
                                entity_context.selected_id = selected_entity->get_id();
                            }
                            context.is_stop = true;
                            context.current_frame = ukey;
                            context.is_current_frame_changed = true;
                            // ImGui::OpenPopup("my_select_popup");
                            ImGui::ItemSelect(name);
                        }
                    }
                    ImGui::EndNeoTimeLine();
                }
            }
            ImGui::EndNeoGroup();
        }
    }

    // void TimelineLayer::draw_keyframe_popup(UiContext &ui_context)
    // {
    //     if (clicked_bone_ != nullptr)
    //     {
    //         // ImGui::ShowDemoWindow()
    //         ImGui::SameLine();
    //         ImGui::SetNextWindowSize({400.0, 90.0});
    //         if (ImGui::BeginPopup("my_select_popup")) //&& !ui_context.entity.is_changed_selected_entity)
    //         {
    //             ImGuiWindow *window = ImGui::GetCurrentWindow();

    //             draw_bone_status(ui_context);

    //             // if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsMouseHoveringRect(window->InnerRect.Min, window->InnerRect.Max))
    //             // {
    //             //     clicked_bone_ = nullptr;
    //             // }
    //             ImGui::EndPopup();
    //         }
    //     }
    // }
    // void TimelineLayer::draw_bone_status(UiContext &ui_context)
    // {
    //     ImGui::Text("%s: %u", clicked_bone_->get_name().c_str(), clicked_frame_);
    //     auto t = clicked_bone_->get_position(clicked_frame_);
    //     auto r = clicked_bone_->get_rotation(clicked_frame_);
    //     auto s = clicked_bone_->get_scale(clicked_frame_);
    //     glm::vec3 tt{}, ss{};
    //     glm::vec4 tmp_r{};
    //     glm::quat rr{};
    //     bool changed = false;

    //     if (t)
    //     {
    //         tt = *t;
    //         changed = DragFPropertyXYZ("Tr", &tt[0], 0.1f, -1000.0f, 1000.0f, "%.3f");
    //     }
    //     if (r)
    //     {
    //         tmp_r = glm::vec4{r->x, r->y, r->z, r->w};
    //         changed |= DragFPropertyXYZ("Rt", &tmp_r[0], 0.01f, -1.0f, 1.0f, "%.3f", "", 4);
    //         rr = glm::quat{tmp_r.a, tmp_r.r, tmp_r.g, tmp_r.b};
    //         rr = glm::normalize(rr);
    //     }
    //     if (s)
    //     {
    //         ss = *s;
    //         changed |= DragFPropertyXYZ("Sc", &ss[0], 0.1f, 0.1f, 179.0f, "%.3f", "");
    //     }
    //     if (changed)
    //     {
    //         // ui_context.entity.is_changed_transform = true;
    //         // ui_context.entity.new_transform = glm::translate(glm::mat4(1.0f), tt) * glm::mat4(rr) * glm::scale(glm::mat4(1.0f), ss);
    //     }
    // }

}
