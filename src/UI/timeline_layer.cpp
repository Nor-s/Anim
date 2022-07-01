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
    bool TimelineLayer::IsHoveredZoomSlider = false;

    TimelineLayer::TimelineLayer()
    {
        text_editor_.reset(new TextEditLayer());
        text_editor_->init();
    }

    void TimelineLayer::draw(Scene *scene, TimelineContext &context)
    {
        // std::vector<const char *> animation_items = animator->get_animation_name_list();

        // uint32_t currentFrame = animator->get_current_frame_num();
        // uint32_t beforeFrame = currentFrame;
        // uint32_t startFrame = 0;
        // uint32_t endFrame = animator->get_custom_duration();
        // float &fps = animator->get_mutable_fps();
        // float &tick_per_second = animator->get_mutable_custom_tick_per_second();
        // std::vector<uint32_t> keys = {0, 0, 10, 24};

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize;
        auto entity = scene->get_mutable_selected_entity();
        auto resource = scene->get_mutable_shared_resources().get();

        if (IsHoveredZoomSlider)
        {
            window_flags |= ImGuiWindowFlags_NoScrollWithMouse;
        }
        ImGui::Begin("Animation", NULL, window_flags);
        {
            draw_play_all_button(context);
            ImGui::SameLine();
            draw_play_and_stop_button(context);
            ImGui::SameLine();
            draw_animation_option_button(entity);
            //     float input_fps = fps;
            //     ImGui::InputFloat("fps", &input_fps);
            //     fps = input_fps;

            //     ImGui::SameLine();
            //     float input_tick_per_second = tick_per_second;
            //     ImGui::InputFloat("scale", &input_tick_per_second);
            //     tick_per_second = input_tick_per_second;
            // }
            // if (beforeFrame != currentFrame)
            // {
            //     animator->set_current_frame_num_to_time(currentFrame);
            // }
        }
        ImGui::End();
    }
    void TimelineLayer::draw_sequencer(glcpp::Animation *current_animation)
    {
        //     if (ImGui::BeginNeoSequencer("Sequencer", &currentFrame, &startFrame, &endFrame))
        //     {
        //         draw_keyframes();
        //         draw_keyframe_popup();

        //         ImGui::EndNeoGroup();
        //         if (ImGui::IsZoomSliderHovered())
        //         {
        //             is_hovered_animation_zoom_slider = true;
        //         }
        //         else
        //         {
        //             is_hovered_animation_zoom_slider = false;
        //         }
        //     }
        //     ImGui::EndNeoSequencer();
        // }
    }
    void TimelineLayer::draw_keyframes()
    {
        // if (ImGui::BeginNeoGroup("Transform", &m_pTransformOpen))
        // {
        //     auto &name_bone_map = current_animation->get_mutable_name_bone_map();
        //     bool is_hovered = false;
        //     for (auto &bone : name_bone_map)
        //     {
        //         float factor = bone.second->get_factor();
        //         std::vector<float> &keys = bone.second->get_mutable_time_list();

        //         if (ImGui::BeginNeoTimeline(bone.second->get_bone_name().c_str()))
        //         {
        //             for (size_t i = 0; i < keys.size(); i++)
        //             {
        //                 uint32_t key = static_cast<uint32_t>(roundf(keys[i] * factor));
        //                 if (ImGui::Keyframe(&key, &is_hovered) && is_hovered && clicked_time == -1.0f)
        //                 {
        //                     if (ImGui::IsItemClicked())
        //                     {
        //                         clicked_frame = key;
        //                         clicked_time = keys[i];
        //                         clicked_bone = bone.second.get();
        //                     }
        //                 }
        //             }

        //             ImGui::EndNeoTimeLine();
        //         }
        //     }
    }
    void TimelineLayer::draw_keyframe_popup()
    {
        // if (clicked_time != -1.0f && clicked_bone)
        // {
        //     ImVec2 vMin{0, 0};
        //     ImVec2 vMax{0, 0};
        //     currentFrame = clicked_frame;
        //     is_stop = true;
        //     ImGui::OpenPopup("my_select_popup");
        //     ImGui::SameLine();
        //     if (ImGui::BeginPopup("my_select_popup"))
        //     {
        //         vMin = ImGui::GetWindowContentRegionMin();
        //         vMax = ImGui::GetWindowContentRegionMax();
        //         vMin.x += ImGui::GetWindowPos().x - 10;
        //         vMin.y += ImGui::GetWindowPos().y - 10;
        //         vMax.x += ImGui::GetWindowPos().x + 10;
        //         vMax.y += ImGui::GetWindowPos().y + 10;
        //         ImGui::Text("%s: %u", clicked_bone->get_bone_name().c_str(), clicked_frame);
        //         glm::vec3 *p_pos = clicked_bone->get_mutable_pointer_positions(clicked_time);
        //         glm::quat *p_quat = clicked_bone->get_mutable_pointer_rotations(clicked_time);
        //         glm::vec3 *p_scale = clicked_bone->get_mutable_pointer_scales(clicked_time);
        //         if (p_pos)
        //         {
        //             ImGui::SliderFloat3("position", &((*p_pos)[0]), 0.0f, 50.0f);
        //         }
        //         if (p_quat)
        //         {
        //             auto before_quat = *p_quat;
        //             ImGui::SliderFloat4("quaternion", &(*p_quat)[0], -1.0f, 1.0f);
        //             if (before_quat != *p_quat)
        //             {
        //                 *p_quat = glm::normalize(*p_quat);
        //             }
        //         }
        //         if (p_scale)
        //         {
        //             ImGui::SliderFloat3("scale", &((*p_scale)[0]), 0.1f, 50.0f);
        //         }

        //         ImGui::EndPopup();
        //     }
        //     if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        //     {
        //         auto x = ImGui::GetMousePos().x;
        //         auto y = ImGui::GetMousePos().y;
        //         if (!(vMax.y > y && vMax.x > x && vMin.x < x && vMin.y < y))
        //         {
        //             clicked_time = -1.0f;
        //         }
        //     }
        // }
    }

    void TimelineLayer::draw_play_all_button(TimelineContext &context)
    {
        if (ImGui::Button("All play"))
        {
            context.is_clicked_play_all = true;
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

    void TimelineLayer::draw_animation_option_button(glcpp::Entity *entity)
    {
        static bool edit_open = false;
        glcpp::AnimationComponent *animation_component = nullptr;
        if (entity && entity->has_animation_component())
        {
            animation_component = entity->get_mutable_pointer_animation_component();
            bool *p_is_loop = animation_component->get_mutable_pointer_is_loop();
            ImGui::Checkbox("loop", p_is_loop);

            if (animation_component->get_animation()->get_type() == glcpp::AnimationType::Json)
            {
                ImGui::SameLine();

                if (ImGui::Button("edit"))
                {
                    text_editor_->open(animation_component->get_mutable_animation()->get_name());
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
    }

    void TimelineLayer::draw_animation_list(TimelineContext &context, const SharedResources *shared_resources, const glcpp::Entity *entity)
    {
        const auto &animations = shared_resources->get_animations();
        const char *current_animation_name = nullptr;
        if (entity->has_animation_component())
        {
            current_animation_name = entity->get_pointer_animation_component()->get_animation()->get_name();
        }

        if (ImGui::BeginCombo("animations", current_animation_name))
        {
            for (size_t i = 0; i < animations.size(); i++)
            {
                const char *animation_name = animations[i]->get_name();

                bool is_selected = false;
                if (current_animation_name != nullptr)
                {
                    is_selected = (strcmp(current_animation_name, animation_name) == 0);
                }
                if (ImGui::Selectable(animation_name, is_selected))
                {
                    std::cout << "clicke\n";
                    current_animation_name = animation_name;
                    context.animation_idx = i;
                }
                if (is_selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
    }
}