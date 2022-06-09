#ifndef UI_IMGUI_CONTEXT_H
#define UI_IMGUI_CONTEXT_H

#include <glm/glm.hpp>
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_glfw.h>
#include "imgui/imgui_neo_sequencer.h"
#include <nfd.h>
#include <iostream>
#include <glcpp/shader.h>
#include <glcpp/camera.h>
#include <glcpp/model.h>
#include <glcpp/window.h>
#include <glcpp/cubemap.h>
#include <glcpp/framebuffer.h>
#include <glcpp/anim/animation.hpp>
#include <glcpp/anim/animator.hpp>
#include "pixelate_framebuffer.h"
#include <memory>
#include <map>
#include <thread>
#include <filesystem>

#include "imgui_scene_window.h"
#include "imgui_text_edit.h"
#include "scene/scene.hpp"
#include "imgui_json.h"
#include "glcpp/utility.hpp"

#include <atomic>

struct GLFWwindow;

static void executeProcess(const char *process_name, std::string arg, std::atomic<bool>& done)
{
#ifndef NDEBUG    
    std::cout << "begin: executeProcess-----------------\n";
#endif
    std::filesystem::path process = std::filesystem::u8path(process_name);
    std::string abs_process = std::filesystem::absolute(process).string();
#ifdef _WIN32    
    abs_process += ".exe";
#endif   
    abs_process += " " + arg;   
    try {
        system(abs_process.c_str());
#ifndef NDEBUG    
        std::cout << "end: executeProcess-----------------" << std::endl;
#endif
        done = true;
    }
    catch (std::exception& e) {
#ifndef NDEBUG    

        std::cout << e.what()<<std::endl;
#endif

        done = true;
    }
}


namespace ui
{

    static void HelpMarker(const char *desc)
    {
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }

    class ImGuiContext
    {
    public:
        virtual ~ImGuiContext()
        {
            shutdown();
        }
        void shutdown()
        {
            if (thread_for_process_open_.joinable())
            {
                thread_for_process_open_.detach();
            }
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
        }
        void init(GLFWwindow *window)
        {
            NFD_Init();
            const char *glsl_version = "#version 330";

            IMGUI_CHECKVERSION();
            ImGui::CreateContext();

            ImGuiIO &io = ImGui::GetIO();
            (void)io;
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
            ImGui::StyleColorsLight();
            ImGuiStyle &style = ImGui::GetStyle();
            set_style();
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                style.WindowRounding = 0.0f;
                style.Colors[ImGuiCol_WindowBg].w = 1.0f;
            }
            io.Fonts->AddFontFromFileTTF("./resources/font/D2Coding.ttf", 16.0f, NULL, io.Fonts->GetGlyphRangesKorean());

            // Setup Platform/Renderer backends
            ImGui_ImplGlfw_InitForOpenGL(window, true);
            ImGui_ImplOpenGL3_Init(glsl_version);

            text_editor_.init();
        }
        void set_style()
        {
            ImGuiStyle *style = &ImGui::GetStyle();
            style->GrabRounding = style->FrameRounding = 3.0f;
            style->WindowPadding = {3.0f, 3.0f};
            style->WindowBorderSize = style->FrameBorderSize = style->PopupBorderSize = 1.0f;

            // style->Colors[ImGuiCol_WindowBg] = ImVec4{0.7f, 0.7f, 0.7f, 1.0f};
            // style->Colors[ImGuiCol_Header] = ImVec4{0.2f, 0.2f, 0.2f, 1.0f};
            // style->Colors[ImGuiCol_HeaderHovered] = ImVec4{0.3f, 0.3f, 0.3f, 1.0f};
            // style->Colors[ImGuiCol_HeaderActive] = ImVec4{0.15f, 0.15f, 0.15f, 1.0f};
            // style->Colors[ImGuiCol_Button] = ImVec4{0.5f, 0.5f, 0.5f, 1.0f};
            // style->Colors[ImGuiCol_ButtonHovered] = ImVec4{0.3f, 0.3f, 0.3f, 1.0f};
            // style->Colors[ImGuiCol_ButtonActive] = ImVec4{0.15f, 0.15f, 0.15f, 1.0f};
            // style->Colors[ImGuiCol_FrameBg] = ImVec4{0.5f, 0.5f, 0.5f, 1.0f};
            // style->Colors[ImGuiCol_FrameBgHovered] = ImVec4{0.3f, 0.3f, 0.3f, 1.0f};
            // style->Colors[ImGuiCol_FrameBgActive] = ImVec4{0.15f, 0.15f, 0.15f, 1.0f};
            // style->Colors[ImGuiCol_Tab] = ImVec4{0.15f, 0.15f, 0.15f, 1.0f};
            // style->Colors[ImGuiCol_TabHovered] = ImVec4{0.38f, 0.38f, 0.38f, 1.0f};
            // style->Colors[ImGuiCol_TabActive] = ImVec4{0.28f, 0.28f, 0.28f, 1.0f};
            // style->Colors[ImGuiCol_TabUnfocused] = ImVec4{0.15f, 0.15f, 0.15f, 1.0f};
            // style->Colors[ImGuiCol_TabUnfocusedActive] = ImVec4{0.2f, 0.2f, 0.2f, 1.0f};
            // style->Colors[ImGuiCol_TitleBg] = ImVec4{0.15f, 0.15f, 0.15f, 1.0f};
            // style->Colors[ImGuiCol_TitleBgActive] = ImVec4{0.15f, 0.15f, 0.15f, 1.0f};
            // style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4{0.15f, 0.15f, 0.15f, 1.0f};

            // style->WindowPadding = ImVec2(15, 15);
            // style->WindowRounding = 5.0f;
            // style->FramePadding = ImVec2(5, 5);
            // style->FrameRounding = 4.0f;
            // style->ItemSpacing = ImVec2(12, 8);
            // style->ItemInnerSpacing = ImVec2(8, 6);
            // style->IndentSpacing = 25.0f;
            // style->ScrollbarSize = 15.0f;
            // style->ScrollbarRounding = 9.0f;
            // style->GrabMinSize = 5.0f;
            // style->GrabRounding = 3.0f;

            // style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
            // style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
            // style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
            // style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
            // style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
            // style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
            // style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
            // style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
            // style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
            // style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
            // style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
            // style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
            // style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
            // style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
            // style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
            // style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
            // style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
            // style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
            // style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
            // style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
            // style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
        }

        void begin()
        {
            // feed inputs to dear imgui, start new frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
        }
        void draw_dock()
        {

            bool s = true;
            ShowExampleAppDockSpace(&s);
        }
        const char *dfs(std::shared_ptr<glcpp::ModelNode> &anim_node, const ImGuiTreeNodeFlags &node_flags, int &count)
        {
            static int selected_idx = 0;
            const char *selected_node = nullptr;
            auto selected_flags = node_flags;
            count++;
            if (selected_idx == count)
            {
                selected_flags |= ImGuiTreeNodeFlags_Selected;
            }
            if (anim_node->childrens.size() == 0)
            {
                selected_flags |= ImGuiTreeNodeFlags_Leaf;
            }
            bool node_open = ImGui::TreeNodeEx(anim_node->name.c_str(), selected_flags);
            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
            {
                selected_idx = count;
                selected_node = anim_node->name.c_str();
            }

            if (node_open)
            {
                for (size_t i = 0; i < anim_node->childrens.size(); i++)
                {
                    const char *selected_child_node = dfs(anim_node->childrens[i], node_flags, count);
                    if (selected_child_node != nullptr)
                    {
                        selected_node = selected_child_node;
                    }
                }
                ImGui::TreePop();
            }
            return selected_node;
        }
        void draw_model_hierarchy(std::shared_ptr<glcpp::ModelNode> &root_node, glcpp::Animator *animator)
        {
            static const char *selected_node_name = nullptr;
            ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
            int node_count = 0;
            ImGui::Begin("Model Hierarchy");
            auto node_name = dfs(root_node, node_flags, node_count);
            if (node_name)
            {
                selected_node_name = node_name;
            }
            if (selected_node_name)
            {
                ImGui::BeginChild("Bone property", {0, 100}, true);
                ImGui::Text("%s", selected_node_name);
                glcpp::Bone *bone = animator->get_mutable_current_animation()->FindBone(selected_node_name);
                if (bone)
                {
                    // TODO: FIX BUG: pointer error
                    glm::vec3 *p_pos = bone->get_mutable_pointer_recently_used_position();
                    glm::quat *p_quat = bone->get_mutable_pointer_recently_used_rotation();
                    glm::vec3 *p_scale = bone->get_mutable_pointer_recently_used_scale();
                    if (p_pos)
                    {
                        ImGui::SliderFloat3("position", &(*p_pos)[0], -10.0f, 10.0f);
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
                        ImGui::SliderFloat3("scale", &(*p_scale)[0], 10.0f, -10.0f);
                    }
                }
                else
                {
                    ImGui::Text("Can't find bone");
                }
                ImGui::EndChild();
            }
            ImGui::End();
        }
        void draw_animation_bar(Scene *scene)
        {
            static bool is_hovered_animation_zoom_slider = false;
            static bool m_pTransformOpen = true;
            static std::string play_stop_button = "stop";
            static const char *current_item = nullptr;
            static float clicked_time = -1.0f;
            static uint32_t clicked_frame = 0;
            static glcpp::Bone *clicked_bone = nullptr;
            static bool is_load_animation = false;
            glcpp::Animator *animator = scene->get_mutable_animator();
            bool &play = animator->get_mutable_is_stop();
            std::vector<const char *> animation_items = animator->get_animation_name_list();
            bool is_json = (animator->get_mutable_current_animation()->get_type() == glcpp::AnimationType::Json);
            uint32_t currentFrame = animator->get_current_frame_num();
            uint32_t beforeFrame = currentFrame;
            uint32_t startFrame = 0;
            uint32_t endFrame = animator->get_custom_duration();
            float &fps = animator->get_mutable_custom_tick_per_second();
            std::vector<uint32_t> keys = {0, 0, 10, 24};
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize;
            if (is_hovered_animation_zoom_slider)
            {
                window_flags |= ImGuiWindowFlags_NoScrollWithMouse;
            }
            try
            {
                ImGui::Begin("Animation bar", NULL, window_flags);
                if (ImGui::Button(play_stop_button.c_str()))
                {
                    play = !play;
                    if (play_stop_button == "play")
                    {
                        play_stop_button = "stop";
                    }
                    else
                    {
                        play_stop_button = "play";
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("load animation") && !is_load_animation)
                {
                    is_load_animation = true;
                    nfdchar_t *outPath;
                    nfdfilteritem_t filterItem[1] = {{"file", "dae,fbx,json,md5anim,collada,gltf"}};
                    nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 1, NULL);

                    if (result == NFD_OKAY)
                    {
                        animator->add_animation(outPath);
                        NFD_FreePath(outPath);
                    }
                    else if (result == NFD_CANCEL)
                    {
                    }
                    else
                    {
                    }

                    NFD_Quit();
                    is_load_animation = false;
                }
                ImGui::SameLine();
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
                if (is_json)
                {
                    ImGui::SameLine();
                    static bool edit_open = false;
                    if (ImGui::Button("edit"))
                    {
                        text_editor_.open(animator->get_mutable_current_animation()->get_name());
                        edit_open = true;
                    }
                    text_editor_.draw(&edit_open);

                    ImGui::SameLine();
                    if (ImGui::Button("reload"))
                    {
                        animator->get_mutable_current_animation()->reload();
                    }
                }
                ImGui::SameLine();
                float input_fps = fps;
                ImGui::InputFloat("fps", &input_fps);
                fps = input_fps;
                ImGui::SameLine();
                if (ImGui::Button("Mediapipe Open"))
                {
                    execute_process("./mp2mm/mp2mm", scene);
                }
                ImGui::SameLine();

                if (ImGui::Button("export: gltf"))
                {
                    nfdchar_t* out_path;
                    nfdfilteritem_t filter_item[1] = { {"anim", "gltf"} };
                    nfdresult_t result = NFD_SaveDialog(&out_path, filter_item, 1, NULL, "anim.gltf");

                    if (result == NFD_OKAY)
                    {
                        scene->to_fbx(out_path);
                        NFD_FreePath(out_path);
                    }
                    else if (result != NFD_CANCEL)
                    {
                    }

                    NFD_Quit();
                }
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
                            play = true;
                            play_stop_button = "play";
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

#ifndef NDEBUG
            ImGui::ShowDemoWindow();
#endif
        }
        void draw_property(Scene *scene)
        {
            static int count = 0;
            ImGui::Begin("Debug");
            {
                if (ImGui::Button("get binding pose json"))
                {
                    ImguiJson::ModelBindingPoseToJson("model.json", scene->get_model().get());
                }
                if (ImGui::Button("get animation to json"))
                {
                    ImguiJson::AnimationToJson("animation.json", scene->get_mutable_animator()->get_mutable_current_animation().get());
                }
            }
            ImGui::End();

            // render your GUI
            ImGui::Begin("Model Property");
            {
                if (ImGui::Button("print"))
                {
                    scene->print_to_png("pixel" + std::to_string(count++) + ".png");
                }

                ImGui::SameLine();
                process_option(scene->get_option());

                process_model(scene->get_model()->get_mutable_transform());
                // open Dialog Simple
                if (ImGui::Button("load"))
                {
                    nfdchar_t *out_path;
                    nfdfilteritem_t filter_item[1] = {{"model file", "obj,dae,pmx,fbx,md5mesh,gltf"}};
                    nfdresult_t result = NFD_OpenDialog(&out_path, filter_item, 1, NULL);

                    if (result == NFD_OKAY)
                    {
                        scene->add_model(out_path);
                        ImguiJson::ModelBindingPoseToJson("model.json", scene->get_model().get());
                        NFD_FreePath(out_path);
                    }
                    else if (result != NFD_CANCEL)
                    {
                    }

                    NFD_Quit();
                }
            }
            ImGui::End();

            draw_model_hierarchy(scene->get_model()->get_mutable_root_node(), scene->get_mutable_animator());
            draw_animation_bar(scene);
        }
        void process_option(ImguiOption &imgui_options)
        {
            auto &flags = imgui_options.get_flags();
            for (auto &flag : flags)
            {
                if (ImGui::Button(flag.first.c_str()))
                {
                    flag.second = !flag.second;
                }
            }
            auto &int_properties = imgui_options.get_int_properties();
            for (auto &property : int_properties)
            {
                auto &[name, value, min_value, max_value] = property;
                ImGui::Text("%s", name.c_str());
                ImGui::PushItemWidth(-1);
                ImGui::SliderInt(name.c_str(), &value, min_value, max_value);
                ImGui::PopItemWidth();
            }

            auto &color3_properties = imgui_options.get_color3_property();
            for (auto &property : color3_properties)
            {
                std::string title = property.first;
                ImGui::Text("%s", title.c_str());
                ImGui::PushItemWidth(-1);
                ImGui::ColorEdit3(property.first.c_str(), &property.second[0]);
                ImGui::PopItemWidth();
            }
        }
        // Demonstrate creating a window covering the entire screen/viewport
        void ShowStatus(float fps)
        {
            static ImGuiWindowFlags flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration;

            // We demonstrate using the full viewport area or the work area (without menu-bars, task-bars etc.)
            // Based on your use case you may want one of the other.
            bool tmp = true;

            if (ImGui::Begin("fps", &tmp, flags))
            {
                ImGui::Text("fps: %f", fps);
            }
            ImGui::End();
        }
        void process_model(glcpp::TransformComponent &transform)
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
        void end()
        {
            // Render dear imgui into screen
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            ImGuiIO &io = ImGui::GetIO();
            // Update and Render additional Platform Windows
            // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
            //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                GLFWwindow *backup_current_context = glfwGetCurrentContext();
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                glfwMakeContextCurrent(backup_current_context);
            }
        }

        void ShowExampleAppDockSpace(bool *p_open)
        {
            // If you strip some features of, this demo is pretty much equivalent to calling DockSpaceOverViewport()!
            // In most cases you should be able to just call DockSpaceOverViewport() and ignore all the code below!
            // In this specific demo, we are not using DockSpaceOverViewport() because:
            // - we allow the host window to be floating/moveable instead of filling the viewport (when opt_fullscreen == false)
            // - we allow the host window to have padding (when opt_padding == true)
            // - we have a local menu bar in the host window (vs. you could use BeginMainMenuBar() + DockSpaceOverViewport() in your code!)
            // TL;DR; this demo is more complicated than what you would normally use.
            // If we removed all the options we are showcasing, this demo would become:
            //     void ShowExampleAppDockSpace()
            //     {
            //         ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
            //     }

            static bool opt_fullscreen = true;
            static bool opt_padding = false;
            static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

            // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
            // because it would be confusing to have two docking targets within each others.
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
            if (opt_fullscreen)
            {
                const ImGuiViewport *viewport = ImGui::GetMainViewport();
                ImGui::SetNextWindowPos(viewport->WorkPos);
                ImGui::SetNextWindowSize(viewport->WorkSize);
                ImGui::SetNextWindowViewport(viewport->ID);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
                window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
                window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
            }
            else
            {
                dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
            }

            // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
            // and handle the pass-thru hole, so we ask Begin() to not render a background.
            if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
                window_flags |= ImGuiWindowFlags_NoBackground;

            // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
            // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
            // all active windows docked into it will lose their parent and become undocked.
            // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
            // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
            if (!opt_padding)
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::Begin("DockSpace Demo", p_open, window_flags);
            if (!opt_padding)
                ImGui::PopStyleVar();

            if (opt_fullscreen)
                ImGui::PopStyleVar(2);

            // Submit the DockSpace
            ImGuiIO &io = ImGui::GetIO();
            if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
            {
                ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
                ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
            }
            else
            {
                // ShowDockingDisabledMessage();
            }

            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("Options"))
                {
                    // Disabling fullscreen would allow the window to be moved to the front of other windows,
                    // which we can't undo at the moment without finer window depth/z control.
                    ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
                    ImGui::MenuItem("Padding", NULL, &opt_padding);
                    ImGui::Separator();

                    if (ImGui::MenuItem("Flag: NoSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0))
                    {
                        dockspace_flags ^= ImGuiDockNodeFlags_NoSplit;
                    }
                    if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0))
                    {
                        dockspace_flags ^= ImGuiDockNodeFlags_NoResize;
                    }
                    if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0))
                    {
                        dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode;
                    }
                    if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0))
                    {
                        dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar;
                    }
                    if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen))
                    {
                        dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode;
                    }
                    ImGui::Separator();

                    if (ImGui::MenuItem("Close", NULL, false, p_open != NULL))
                        *p_open = false;
                    ImGui::EndMenu();
                }
                HelpMarker(
                    "When docking is enabled, you can ALWAYS dock MOST window into another! Try it now!"
                    "\n"
                    "- Drag from window title bar or their tab to dock/undock."
                    "\n"
                    "- Drag from window menu button (upper-left button) to undock an entire node (all windows)."
                    "\n"
                    "- Hold SHIFT to disable docking (if io.ConfigDockingWithShift == false, default)"
                    "\n"
                    "- Hold SHIFT to enable docking (if io.ConfigDockingWithShift == true)"
                    "\n"
                    "This demo app has nothing to do with enabling docking!"
                    "\n\n"
                    "This demo app only demonstrate the use of ImGui::DockSpace() which allows you to manually create a docking node _within_ another window."
                    "\n\n"
                    "Read comments in ShowExampleAppDockSpace() for more details.");

                ImGui::EndMenuBar();
            }

            ImGui::End();
        }
        void draw_scene(const std::string &title, Scene *scene)
        {
            if (scene_map_.find(title) == scene_map_.end())
            {
                scene_map_[title] = std::make_unique<ImguiSceneWindow>();
            }
            scene_map_[title]->draw(title.c_str(), scene);
        }

        bool is_window_hovered(const std::string &title)
        {
            if (scene_map_.find(title) == scene_map_.end())
            {
                return false;
            }
            return scene_map_[title]->get_is_hovered();
        }
        void draw_texture(const char *title, glcpp::Framebuffer &framebuffer)
        {
            ImGui::Begin(title);
            float width = (float)framebuffer.get_width();
            float height = (float)framebuffer.get_height();
            ImGui::Image(reinterpret_cast<ImTextureID>(framebuffer.get_color_texture()), ImVec2{width, height}, ImVec2{0, 1}, ImVec2{1, 0});
            ImGui::End();
        }

        bool execute_process(const char *process_name, Scene *scene)
        {
#ifndef NDEBUG
            std::cout << "click btn\n";
#endif
            if (atomic_for_process_open_)
            {  
                atomic_for_process_open_ = false;
                if (thread_for_process_open_.joinable()) {
#ifndef NDEBUG
                    std::cout << "begin: thread.join\n";
#endif
                    thread_for_process_open_.join();
#ifndef NDEBUG
                    std::cout << "end: thread.join\n";
#endif
                }
                ImguiJson::ModelBindingPoseToJson("model.json", scene->get_model().get());

                std::filesystem::path path_default_model = std::filesystem::u8path("model.json");
                std::filesystem::path path_default_anim = std::filesystem::u8path( "anim.json");
                std::string arg = "--arg1 " + std::filesystem::absolute(path_default_model).string() + " --arg2 " +
                                  std::filesystem::absolute(path_default_anim).string();
               
                thread_for_process_open_ = std::thread(executeProcess, process_name, arg, std::ref(atomic_for_process_open_));

                return true;
            }
            else {
#ifndef NDEBUG

                std::cout << "The process is running.\n";
#endif
            }
            return false;
        }

    private:
        std::map<std::string, std::unique_ptr<ImguiSceneWindow>> scene_map_;
        ImGuiTextEditor text_editor_;
        std::thread thread_for_process_open_;
        std::atomic<bool> atomic_for_process_open_{true};
    };
}

#endif
