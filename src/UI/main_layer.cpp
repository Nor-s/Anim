#include "main_layer.h"
#include "imgui_helper.hpp"
#include "hierarchy_layer.h"
#include "scene_layer.h"
#include "model_property_layer.h"
#include "timeline_layer.h"
#include "text_edit_layer.h"

#include <glcpp/window.h>

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_glfw.h>
#include <nfd.h>

namespace ui
{
    MainLayer::MainLayer() = default;
    
    MainLayer::~MainLayer() = default;

    void MainLayer::init(GLFWwindow *window)
    {
        NFD_Init();
        const char *glsl_version = "#version 430";

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
        }
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        style.WindowPadding.x = 3.0f;
        style.WindowPadding.y = 3.0f;
        style.FramePadding.y = 1.0f;
        io.Fonts->AddFontFromFileTTF("./resources/font/D2Coding.ttf", 16.0f, NULL, io.Fonts->GetGlyphRangesKorean());

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);
        init_layer();
    }

    void MainLayer::init_layer()
    {
        hierarchy_layer_ = std::make_unique<HierarchyLayer>();
        property_layer_ = std::make_unique<ModelPropertyLayer>();
        timeline_layer_ = std::make_unique<TimelineLayer>();
    }

    void MainLayer::shutdown()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void MainLayer::begin()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        context_ = UiContext{};
    }

    void MainLayer::end()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        ImGuiIO &io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow *backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }

    void MainLayer::draw_dock(float fps)
    {
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        ImGui::Begin("DockSpace Demo", NULL, window_flags);
        ImGui::PopStyleVar();

        ImGui::PopStyleVar(2);

        // Submit the DockSpace
        ImGuiIO &io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }
        draw_menu_bar(fps);
        ImGui::End();
    }

    void MainLayer::draw_menu_bar(float fps)
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Import:model", NULL, nullptr))
                {
                    context_.menu_context.clicked_import_model = true;
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Export:animation", NULL, nullptr))
                {
                    context_.menu_context.clicked_export_animation = true;
                }

                ImGui::EndMenu();
            }
            HelpMarker(
                "Import: fbx, glft2, obj ..."
                "\n"
                "Export: {animation glft2}");

            ImGui::Text("fps: %.2f", fps);

            ImGui::EndMenuBar();
        }
    }

    void MainLayer::draw_scene(const std::string &title, Scene *scene)
    {
        if (scene_layer_map_.find(title) == scene_layer_map_.end())
        {
            scene_layer_map_[title] = std::make_unique<SceneLayer>();
        }
        scene_layer_map_[title]->draw(title.c_str(), scene);
    }

    void MainLayer::draw_model_properties(glcpp::Entity *entity)
    {
        property_layer_->draw(entity);
    }

    void MainLayer::draw_hierarchy_layer(glcpp::Entity *entity)
    {
        hierarchy_layer_->draw(entity);
    }

    bool MainLayer::is_scene_layer_hovered(const std::string &title)
    {
        if (scene_layer_map_.find(title) == scene_layer_map_.end())
        {
            return false;
        }
        return scene_layer_map_[title]->get_is_hovered();
    }
    const UiContext &MainLayer::get_context() const
    {
        return context_;
    }
}