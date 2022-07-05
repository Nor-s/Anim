#include "main_layer.h"
#include "imgui_helper.hpp"
#include "scene_layer.h"

#include "text_edit_layer.h"

#include <glcpp/window.h>

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_glfw.h>
#include <nfd.h>
#include "ImFileDialog.h"

namespace ui
{
    MainLayer::MainLayer() = default;

    MainLayer::~MainLayer() = default;

    void MainLayer::init(GLFWwindow *window)
    {
        ifd::FileDialog::Instance().CreateTexture = [](uint8_t *data, int w, int h, char fmt) -> void *
        {

            GLuint tex;

            glGenTextures(1, &tex);
            glBindTexture(GL_TEXTURE_2D, tex);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, (fmt == 0) ? GL_BGRA : GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, 0);

            return reinterpret_cast<void *>(tex);
        };

        ifd::FileDialog::Instance().DeleteTexture = [](void *tex)
        {
            GLuint texID = static_cast<GLuint>(reinterpret_cast<uintptr_t>(tex));
            glDeleteTextures(1, &texID);

        };
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
#ifndef NDEBUG

        auto error = glGetError();
        if (error != GL_NO_ERROR)
        {
            std::cout << "ImGui Render: " << error << std::endl;
        }
#endif
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

        if (ifd::FileDialog::Instance().IsDone("Import"))
        {
            if (ifd::FileDialog::Instance().HasResult())
            {
                context_.menu_context.path = ifd::FileDialog::Instance().GetResult().u8string();
                context_.menu_context.clicked_import_model = true;

            }
            ifd::FileDialog::Instance().Close();
        }
    }

    void MainLayer::draw_menu_bar(float fps)
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Import: model, animation", NULL, nullptr))
                {

                    ifd::FileDialog::Instance().Open("Import", "import", "model {.obj,.dae,.pmx,.fbx,.md5mesh,.gltf,.json},.*");

                }
                ImGui::Separator();
                if (ImGui::MenuItem("Export: animation", NULL, nullptr))
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

    void MainLayer::draw_model_properties(Scene *scene)
    {
        property_layer_.draw(context_.properties_context, scene);
    }

    void MainLayer::draw_hierarchy_layer(glcpp::Entity *entity)
    {
        hierarchy_layer_.draw(entity);
    }
    void MainLayer::draw_timeline(Scene *scene)
    {
        timeline_layer_.draw(scene, context_.timeline_context);
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