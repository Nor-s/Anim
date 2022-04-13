#ifndef UI_IMGUI_CONTEXT_H
#define UI_IMGUI_CONTEXT_H

#include <glm/glm.hpp>
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_glfw.h>
#include <nfd.h>
#include <iostream>
#include <glcpp/shader.h>
#include <glcpp/camera.h>
#include <glcpp/model.h>
#include <glcpp/window.h>
#include <glcpp/cubemap.h>
#include <glcpp/framebuffer.h>
#include "pixelate_framebuffer.h"
#include <memory>
#include <map>

#include "imgui_scene_window.h"
#include "scene/scene.hpp"

struct GLFWwindow;

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
        ~ImGuiContext()
        {
            shutdown();
        }
        void shutdown()
        {
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
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                style.WindowRounding = 0.0f;
                style.Colors[ImGuiCol_WindowBg].w = 1.0f;
            }

            // Setup Platform/Renderer backends
            ImGui_ImplGlfw_InitForOpenGL(window, true);
            ImGui_ImplOpenGL3_Init(glsl_version);
        }
        void set_color()
        {
            // ImGuiStyle *style = &ImGui::GetStyle();
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
        void draw_model_hierarchy()
        {
            ImGui::Begin("Model Hierarchy");
            if (ImGui::TreeNode("Trees"))
            {
                if (ImGui::TreeNode("Basic trees"))
                {
                    for (int i = 0; i < 5; i++)
                    {
                        // Use SetNextItemOpen() so set the default state of a node to be open. We could
                        // also use TreeNodeEx() with the ImGuiTreeNodeFlags_DefaultOpen flag to achieve the same thing!
                        if (i == 0)
                            ImGui::SetNextItemOpen(true, ImGuiCond_Once);

                        if (ImGui::TreeNode((void *)(intptr_t)i, "Child %d", i))
                        {
                            ImGui::Text("blah blah");
                            ImGui::SameLine();
                            if (ImGui::SmallButton("button"))
                            {
                            }
                            ImGui::TreePop();
                        }
                    }
                    ImGui::TreePop();
                }
                ImGui::TreePop();
            }
            ImGui::End();
        }
        void draw_property(Scene *scene)
        {
            draw_model_hierarchy();

            // render your GUI
            ImGui::Begin("Model Property");
            {
                // open Dialog Simple
                if (ImGui::Button("load"))
                {
                    nfdchar_t *outPath;
                    nfdfilteritem_t filterItem[2] = {{"model file", "obj,dae,pmx,fbx"}, {"error format", "gltf, dae"}};
                    nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 2, NULL);

                    if (result == NFD_OKAY)
                    {
                        puts("Success!");
                        puts(outPath);
                        scene->add_model(outPath);
                        NFD_FreePath(outPath);
                    }
                    else if (result == NFD_CANCEL)
                    {
                        puts("User pressed cancel.");
                    }
                    else
                    {
                        printf("Error: %s\n", NFD_GetError());
                    }

                    NFD_Quit();
                }
                ImGui::SameLine();
                if (ImGui::Button("print"))
                {
                    scene->print_to_png("pixel.png");
                }
                ImGui::SameLine();
                process_option(scene->get_option());

                process_model(scene->get_model()->get_mutable_transform());
            }
            ImGui::End();
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
        void process_model(glcpp::WorldTransformComponent &transform)
        {
            auto &rotation = transform.get_rotation();
            glm::vec3 r = rotation;
            ImGui::Text("rotation");
            ImGui::SliderFloat("r.x", &r.x, 0.0f, 6.5f);
            ImGui::SliderFloat("r.y", &r.y, 0.0f, 6.5f);
            ImGui::SliderFloat("r.z", &r.z, 0.0f, 6.5f);
            transform.set_rotation({r.x, r.y, r.z});
            auto &scale = transform.get_scale();
            r = scale;
            ImGui::Text("scale");
            ImGui::SliderFloat("s.x", &r.x, 0.1f, 10.0f);
            transform.set_scale({r.x, r.x, r.x});
            auto &translation = transform.get_translation();
            r = translation;
            ImGui::Text("translation");
            ImGui::SliderFloat("t.x", &r.x, -10.0f, 10.0f);
            ImGui::SliderFloat("t.y", &r.y, -10.0f, 10.0f);
            ImGui::SliderFloat("t.z", &r.z, -10.0f, 10.0f);
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
            ImGui::Image(reinterpret_cast<void *>(framebuffer.get_color_texture()), ImVec2{width, height}, ImVec2{0, 1}, ImVec2{1, 0});
            ImGui::End();
        }

    private:
        std::map<std::string, std::unique_ptr<ImguiSceneWindow>> scene_map_;
    };
}

#endif
