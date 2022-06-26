#include "timeline_layer.h"
#include "text_edit_layer.h"
#include "scene/scene.hpp"
#include "glcpp/anim/animator.hpp"
#include "glcpp/anim/bone.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_neo_sequencer.h"

namespace ui
{
    TimelineLayer::TimelineLayer()
    {
        text_editor_.reset(new TextEditLayer());
        text_editor_->init();
    }
    void TimelineLayer::draw(std::vector<std::shared_ptr<Scene>> &scenes, Scene *scene, glcpp::Animator *animator)
    {
        static bool is_hovered_animation_zoom_slider = false;
        static bool m_pTransformOpen = true;
        static const char *current_item = nullptr;
        static float clicked_time = -1.0f;
        static uint32_t clicked_frame = 0;
        static glcpp::Bone *clicked_bone = nullptr;
        static bool is_load_animation = false;
        bool &is_stop = animator->get_mutable_is_stop();
        std::vector<const char *> animation_items = animator->get_animation_name_list();
        bool is_json = (animator->get_mutable_current_animation()->get_type() == glcpp::AnimationType::Json);
        uint32_t currentFrame = animator->get_current_frame_num();
        uint32_t beforeFrame = currentFrame;
        uint32_t startFrame = 0;
        uint32_t endFrame = animator->get_custom_duration();
        float &fps = animator->get_mutable_fps();
        float &tick_per_second = animator->get_mutable_custom_tick_per_second();
        std::vector<uint32_t> keys = {0, 0, 10, 24};
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize;
        if (is_hovered_animation_zoom_slider)
        {
            window_flags |= ImGuiWindowFlags_NoScrollWithMouse;
        }
        try
        {
            ImGui::Begin("Animation bar", NULL, window_flags);
            if (ImGui::Button("All play"))
            {
                for (auto &scene : scenes)
                {
                    animator->get_mutable_is_stop() = false;

                    animator->set_current_frame_num_to_time(0);
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("play"))
            {
                is_stop = false;
                currentFrame = 0;
            }
            ImGui::SameLine();

            if (ImGui::Button("stop"))
            {
                is_stop = true;
            }
            ImGui::SameLine();
            bool *p_is_loop = animator->get_mutable_pointer_is_loop();
            ImGui::Checkbox("loop", p_is_loop);

            ImGui::SameLine();
            if (ImGui::Button("Mediapipe Open"))
            {
                // execute_process("./mp2mm/mp2mm", scene);
            }
            ImGui::SameLine();
            if (is_json)
            {
                ImGui::SameLine();
                static bool edit_open = false;
                if (ImGui::Button("edit"))
                {
                    text_editor_->open(animator->get_mutable_current_animation()->get_name());
                    edit_open = true;
                }
                text_editor_->draw(&edit_open);

                ImGui::SameLine();
                if (ImGui::Button("reload"))
                {
                    animator->reload();
                }
            }
            if (ImGui::BeginCombo("animations", current_item))
            {
                for (size_t i = 0; i < animation_items.size(); i++)
                {

                    bool is_selected = false;
                    if (current_item != nullptr)
                    {
                        is_selected = (strcmp(current_item, animation_items[i]) == 0);
                    }
                    if (ImGui::Selectable(animation_items[i], is_selected))
                    {
                        current_item = animation_items[i];
                        animator->play_animation(i);
                    }
                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::SameLine();
            float input_fps = fps;
            ImGui::InputFloat("fps", &input_fps);
            fps = input_fps;

            ImGui::SameLine();
            float input_tick_per_second = tick_per_second;
            ImGui::InputFloat("scale", &input_tick_per_second);
            tick_per_second = input_tick_per_second;

            if (ImGui::BeginNeoSequencer("Sequencer", &currentFrame, &startFrame, &endFrame))
            {
                // Timeline code here
                if (ImGui::BeginNeoGroup("Transform", &m_pTransformOpen))
                {
                    auto &current_animation = animator->get_mutable_current_animation();
                    auto &name_bone_map = current_animation->get_mutable_name_bone_map();
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
                                if (ImGui::Keyframe(&key, &is_hovered) && is_hovered && clicked_time == -1.0f)
                                {
                                    if (ImGui::IsItemClicked())
                                    {
                                        clicked_frame = key;
                                        clicked_time = keys[i];
                                        clicked_bone = bone.second.get();
                                    }
                                }
                            }

                            ImGui::EndNeoTimeLine();
                        }
                    }
                    if (clicked_time != -1.0f && clicked_bone)
                    {
                        ImVec2 vMin{0, 0};
                        ImVec2 vMax{0, 0};
                        currentFrame = clicked_frame;
                        is_stop = true;
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
                            ImGui::Text("%s: %u", clicked_bone->get_bone_name().c_str(), clicked_frame);
                            glm::vec3 *p_pos = clicked_bone->get_mutable_pointer_positions(clicked_time);
                            glm::quat *p_quat = clicked_bone->get_mutable_pointer_rotations(clicked_time);
                            glm::vec3 *p_scale = clicked_bone->get_mutable_pointer_scales(clicked_time);
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
                                clicked_time = -1.0f;
                            }
                        }
                    }

                    ImGui::EndNeoGroup();
                    if (ImGui::IsZoomSliderHovered())
                    {
                        is_hovered_animation_zoom_slider = true;
                    }
                    else
                    {
                        is_hovered_animation_zoom_slider = false;
                    }
                }
                ImGui::EndNeoSequencer();
            }
            ImGui::End();
            if (beforeFrame != currentFrame)
            {
                animator->set_current_frame_num_to_time(currentFrame);
            }
        }
        catch (std::exception &e)
        {
#ifndef NDEBUG
            std::cout << e.what() << "\n";
#endif
        }
    }
}