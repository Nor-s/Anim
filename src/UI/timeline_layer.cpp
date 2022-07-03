#include "timeline_layer.h"
#include "text_edit_layer.h"
#include "scene/shared_resources.h"
#include "scene/scene.hpp"
#include "glcpp/entity.h"
#include "glcpp/anim/bone.hpp"
#include "glcpp/anim/animation.hpp"
#include "glcpp/component/animation_component.h"

#include <imgui/imgui.h>
#include <imgui/imgui_neo_sequencer.h>

#include <iostream>

namespace ui
{

    TimelineLayer::TimelineLayer()
    {
        text_editor_.reset(new TextEditLayer());
        text_editor_->init();
    }

    void TimelineLayer::draw(Scene *scene, TimelineContext &context)
    {
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize;
        auto entity = scene->get_mutable_selected_entity();
        auto resources = scene->get_mutable_shared_resources().get();
        auto animation_component = entity->get_mutable_pointer_animation_component();

        if (is_hovered_zoom_slider_)
        {
            window_flags |= ImGuiWindowFlags_NoScrollWithMouse;
        }
        ImGui::Begin("Animation", NULL, window_flags);
        {
            if (animation_component && entity->has_bone())
            {
                draw_play_all_button(context);
                ImGui::SameLine();
                draw_play_and_stop_button(context);
                ImGui::SameLine();
                draw_mp2mm(context);
                ImGui::SameLine();
                draw_animation_list(context, resources, entity);
                ImGui::SameLine();
                draw_input_box(context, animation_component);
                ImGui::SameLine();
                draw_animation_option_button(animation_component);
                draw_sequencer(context, animation_component);
            }
            else
            {
                ImGui::Text("can't find bone");
            }
        }
        ImGui::End();
    }

    void TimelineLayer::draw_play_all_button(TimelineContext &context)
    {
        if (ImGui::Button("All play"))
        {
            context.is_clicked_play_all = true;
            context.is_clicked_play = true;
        }
    }

    void TimelineLayer::draw_play_and_stop_button(TimelineContext &context)
    {
        if (ImGui::Button("play"))
        {
            context.is_clicked_play = true;
        }
        ImGui::SameLine();

        if (ImGui::Button("stop"))
        {
            context.is_clicked_stop = true;
        }
    }

    void TimelineLayer::draw_mp2mm(TimelineContext &context)
    {
        if (ImGui::Button("mp2mm"))
        {
            context.is_clicked_mp2mm = true;
        }
    }

    void TimelineLayer::draw_animation_option_button(glcpp::AnimationComponent *animation_component)
    {
        static bool edit_open = false;
        bool *p_is_loop = animation_component->get_mutable_pointer_is_loop();
        ImGui::Checkbox("loop", p_is_loop);
        if (animation_component->get_animation()->get_type() == glcpp::AnimationType::Json)
        {
            ImGui::SameLine();

            if (ImGui::Button("edit"))
            {
                text_editor_->open(animation_component->get_mutable_animation()->get_path());
                edit_open = true;
            }
            text_editor_->draw(&edit_open);
            ImGui::SameLine();

            if (ImGui::Button("reload"))
            {
                animation_component->reload();
            }
        }
    }

    void TimelineLayer::draw_animation_list(TimelineContext &context, const SharedResources *shared_resources, const glcpp::Entity *entity)
    {
        const auto &animations = shared_resources->get_animations();
        const char *current_animation_name = nullptr;
        int current_idx = entity->get_animation_id();
        if (entity->has_animation_component())
        {
            std::string name = std::to_string(current_idx) + ":" + entity->get_pointer_animation_component()->get_animation()->get_name();
            current_animation_name = name.c_str();
        }

        ImGui::PushItemWidth(100);
        ImGui::Text("Animation:");
        ImGui::SameLine();
        if (ImGui::BeginCombo("##animations", current_animation_name))
        {
            for (size_t i = 0; i < animations.size(); i++)
            {
                std::string name = std::to_string(i) + ":" + animations[i]->get_name();
                const char *animation_name = name.c_str();

                bool is_selected = (current_idx == i);
                if (ImGui::Selectable(animation_name, is_selected) && current_idx != i)
                {
                    context.animation_idx = i;
                }
            }
            ImGui::EndCombo();
        }
    }

    void TimelineLayer::draw_input_box(TimelineContext &context, const glcpp::AnimationComponent *animation_component)
    {
        context.fps = animation_component->get_fps();
        context.tps = animation_component->get_tps();
        ImGui::PushItemWidth(30);

        ImGui::Text("Fps:");
        ImGui::SameLine();
        ImGui::DragFloat("##fps", &context.fps, 1.0f, 1.0f, FLT_MAX, "%.0f");
        ImGui::SameLine();
        ImGui::Text("Scale:");
        ImGui::SameLine();
        ImGui::DragFloat("##scale", &context.tps, 1.0f, 1.0f, FLT_MAX, "%.0f");
    }

    void TimelineLayer::draw_sequencer(TimelineContext &context, glcpp::AnimationComponent *animation_component)
    {
        current_frame_ = animation_component->get_current_frame_num();
        uint32_t before_frame = current_frame_;
        uint32_t start_frame = 0;
        uint32_t end_frame = animation_component->get_custom_duration();

        if (ImGui::BeginNeoSequencer("Sequencer", &current_frame_, &start_frame, &end_frame))
        {
            draw_keyframes(animation_component->get_mutable_animation());
            draw_keyframe_popup(context);
            if (ImGui::IsZoomSliderHovered())
            {
                is_hovered_zoom_slider_ = true;
            }
            else
            {
                is_hovered_zoom_slider_ = false;
            }
        }
        ImGui::EndNeoSequencer();

        // update current time
        if (before_frame != current_frame_)
        {
            animation_component->set_current_frame_num_to_time(current_frame_);
        }
    }

    void TimelineLayer::draw_keyframes(glcpp::Animation *animation)
    {
        if (ImGui::BeginNeoGroup("Transform", &is_opened_transform_))
        {
            auto &name_bone_map = animation->get_mutable_name_bone_map();
            bool is_hovered = false;
            for (auto &bone : name_bone_map)
            {
                float factor = bone.second->get_factor();
                std::vector<float> &keys = bone.second->get_mutable_time_list();

                if (ImGui::BeginNeoTimeline(bone.second->get_bone_name().c_str()))
                {
                    for (size_t i = 0; i < keys.size(); i++)
                    {
                        uint32_t key = static_cast<uint32_t>(roundf(keys[i] * factor));
                        if (ImGui::Keyframe(&key, &is_hovered) && is_hovered && clicked_time_ == -1.0f)
                        {
                            if (ImGui::IsItemClicked())
                            {
                                clicked_frame_ = key;
                                clicked_time_ = keys[i];
                                clicked_bone_ = bone.second.get();
                            }
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
        if (clicked_time_ != -1.0f && clicked_bone_)
        {
            ImVec2 vMin{0, 0};
            ImVec2 vMax{0, 0};
            current_frame_ = clicked_frame_;
            context.is_clicked_stop = true;
            ImGui::OpenPopup("my_select_popup");
            ImGui::SameLine();
            if (ImGui::BeginPopup("my_select_popup"))
            {
                vMin = ImGui::GetWindowContentRegionMin();
                vMax = ImGui::GetWindowContentRegionMax();
                vMin.x += ImGui::GetWindowPos().x - 10;
                vMin.y += ImGui::GetWindowPos().y - 10;
                vMax.x += ImGui::GetWindowPos().x + 10;
                vMax.y += ImGui::GetWindowPos().y + 10;
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

                ImGui::EndPopup();
            }
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                auto x = ImGui::GetMousePos().x;
                auto y = ImGui::GetMousePos().y;
                if (!(vMax.y > y && vMax.x > x && vMin.x < x && vMin.y < y))
                {
                    clicked_time_ = -1.0f;
                }
            }
        }
    }
}
