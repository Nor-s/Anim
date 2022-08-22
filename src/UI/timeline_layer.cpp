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

        ImGui::Begin(ICON_MD_SCHEDULE " Animation", 0, window_flags);
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
        context.is_recording = is_recording_;
        if (!context.is_stop)
        {
            context.is_stop = animator_->get_is_stop();
        }
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
            is_recording_ = context.is_recording;
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
        ImGuiIO &io = ImGui::GetIO();
        auto &context = ui_context.timeline;
        uint32_t current = static_cast<uint32_t>(animator_->get_current_time());
        uint32_t start = static_cast<uint32_t>(animator_->get_start_time());
        uint32_t end = static_cast<uint32_t>(animator_->get_end_time());
        uint32_t before = current;
        if (ImGui::BeginNeoSequencer("Sequencer", &current, &start, &end))
        {

            if (root_entity_ && root_entity_->get_component<AnimationComponent>())
            {
                auto anim_component = root_entity_->get_component<AnimationComponent>();
                if (anim_component && anim_component->get_animation())
                {
                    draw_keyframes(ui_context, anim_component->get_animation());
                }
            }
            is_hovered_zoom_slider_ = false;
            if (ImGui::IsZoomSliderHovered())
            {
                is_hovered_zoom_slider_ = true;
            }
            ImGui::EndNeoSequencer();
        }
        // for select keyframe
        // if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
        // {
        //     ImGui::GetForegroundDrawList()->AddRectFilled(io.MouseClickedPos[0],
        //                                                   io.MousePos,
        //                                                   ImGui::GetColorU32(ImGuiCol_Button)); // Draw a line between the button and the mouse cursor
        // }
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
                            auto selected_entity = root_entity_->find(bone.second->get_name());
                            if (selected_entity)
                            {
                                entity_context.is_changed_selected_entity = true;
                                entity_context.selected_id = selected_entity->get_id();
                            }
                            context.is_stop = true;
                            context.current_frame = ukey;
                            context.is_current_frame_changed = true;
                            ImGui::ItemSelect(name);
                        }
                    }
                    ImGui::EndNeoTimeLine();
                }
            }
            ImGui::EndNeoGroup();
        }
    }
}
