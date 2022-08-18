#include "timeline_layer.h"
#include "text_edit_layer.h"
#include "scene/scene.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_neo_sequencer.h>
#include <imgui/icons/icons.h>

#include <entity/entity.h>
#include <resources/shared_resources.h>
#include <animation/animation.h>
#include <animation/animator.h>
#include <animation/bone.h>
#include <entity/components/pose_component.h>
#include <entity/components/animation_component.h>

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

    void TimelineLayer::draw(Scene *scene, TimelineContext &context)
    {
        init_context(context, scene);
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize;

        if (is_hovered_zoom_slider_)
        {
            window_flags |= ImGuiWindowFlags_NoScrollWithMouse;
        }

        ImGui::Begin("Animation", 0, window_flags);
        {
            draw_animator_status(context);
            ImGui::BeginChild("##Timeline", ImVec2(0, 0), false, window_flags);
            {
                draw_sequencer(context);
            }
            ImGui::EndChild();
        }
        ImGui::End();
    }
    inline void TimelineLayer::init_context(TimelineContext &context, Scene *scene)
    {
        scene_ = scene;
        entity_ = scene->get_mutable_selected_entity();
        if (entity_)
        {
            entity_ = entity_->get_mutable_root();
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

    void TimelineLayer::draw_animator_status(TimelineContext &context)
    {
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

    void TimelineLayer::draw_sequencer(TimelineContext &context)
    {
        uint32_t current = static_cast<uint32_t>(animator_->get_current_time());
        uint32_t start = static_cast<uint32_t>(animator_->get_start_time());
        uint32_t end = static_cast<uint32_t>(animator_->get_end_time());
        uint32_t before = current;
        // TODO: begin neo sequencer : uint32_t to int
        if (ImGui::BeginNeoSequencer("Sequencer", &current, &start, &end))
        {
            if (entity_ && entity_->get_component<AnimationComponent>())
            {
                auto anim_component = entity_->get_component<AnimationComponent>();
                if (anim_component && anim_component->get_animation())
                {
                    draw_keyframes(context, anim_component->get_animation());
                    draw_keyframe_popup(context);
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

    void TimelineLayer::draw_keyframes(TimelineContext &context, const Animation *animation)
    {
        if (ImGui::BeginNeoGroup("Transform", &is_opened_transform_))
        {
            auto &name_bone_map = animation->get_name_bone_map();
            bool is_hovered = false;
            for (auto &bone : name_bone_map)
            {
                float factor = bone.second->get_factor();
                std::vector<float> &keys = bone.second->get_mutable_time_list();
                const char *name = bone.second->get_bone_name().c_str();

                if (ImGui::BeginNeoTimeline(name))
                {
                    for (size_t i = 0; i < keys.size(); i++)
                    {
                        uint32_t key = static_cast<uint32_t>(roundf(keys[i] * factor));
                        if (ImGui::Keyframe(&key, &is_hovered) && is_hovered && ImGui::IsItemClicked())
                        {
                            clicked_frame_ = key;
                            clicked_time_ = keys[i];
                            clicked_bone_ = bone.second.get();
                            context.is_clicked_bone = true;
                            context.clicked_bone_name = bone.second->get_bone_name();
                            context.is_stop = true;
                            context.current_frame = clicked_frame_;
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

    void TimelineLayer::draw_keyframe_popup(TimelineContext &context)
    {
        if (clicked_bone_ != nullptr)
        {
            ImGui::OpenPopup("my_select_popup");
            ImGui::SameLine();
            if (ImGui::BeginPopup("my_select_popup") && !context.is_clicked_bone)
            {
                ImGuiWindow *window = ImGui::GetCurrentWindow();

                draw_bone_status();

                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsMouseHoveringRect(window->InnerRect.Min, window->InnerRect.Max))
                {
                    clicked_bone_ = nullptr;
                }
            }
            ImGui::EndPopup();
        }
    }
    void TimelineLayer::draw_bone_status()
    {
        ImGui::Text("%s: %u", clicked_bone_->get_bone_name().c_str(), clicked_frame_);
        glm::vec3 *p_pos = clicked_bone_->get_mutable_pointer_positions(clicked_time_);
        glm::quat *p_quat = clicked_bone_->get_mutable_pointer_rotations(clicked_time_);
        glm::vec3 *p_scale = clicked_bone_->get_mutable_pointer_scales(clicked_time_);
        if (p_pos)
        {
            ImGui::SliderFloat3("position", &((*p_pos)[0]), 0.0f, 50.0f);
        }
        if (p_quat)
        {
            auto before_quat = *p_quat;
            ImGui::SliderFloat4("quaternion", &(*p_quat)[0], -1.0f, 1.0f);
            if (before_quat != *p_quat)
            {
                *p_quat = glm::normalize(*p_quat);
            }
        }
        if (p_scale)
        {
            ImGui::SliderFloat3("scale", &((*p_scale)[0]), 0.1f, 50.0f);
        }
    }

}
