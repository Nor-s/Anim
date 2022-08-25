#include "main_layer.h"
#include "imgui_helper.h"
#include "scene_layer.h"

#include "text_edit_layer.h"

#include <array>
#include <glcpp/window.h>

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/ImGuizmo.h>
#include <imgui/icons/icons.h>
#include <fstream>

#include <ImGuiFileDialog/ImGuiFileDialog.h>
#ifdef _WIN32
#pragma warning(disable : 4005)
#include <Windows.h>
// #include <shellapi.h>
#include <lmcons.h>
// #pragma comment(lib, "Shell32.lib")
#else
// #include <unistd.h>
// #include <pwd.h>
#endif

#define FILTER_MODEL "Model files (*.fbx *.gltf){.fbx,.gltf,.glb}"

namespace ui
{
    static bool isLinear{true};

    MainLayer::MainLayer() = default;

    MainLayer::~MainLayer() = default;

    void MainLayer::init(GLFWwindow *window)
    {
        const char *glsl_version = "#version 330";

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO &io = ImGui::GetIO();
        (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
        ImGui::StyleColorsLight();
        ImGuiStyle &style = ImGui::GetStyle();
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        style.WindowPadding.x = 3.0f;
        style.WindowPadding.y = 3.0f;
        style.FramePadding.y = 1.0f;
        io.Fonts->AddFontFromFileTTF("./resources/font/D2Coding.ttf", 16.0f, NULL, io.Fonts->GetGlyphRangesKorean());
        ImGui::LoadInternalIcons(io.Fonts);
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);
        init_bookmark();
    }
    void MainLayer::init_bookmark()
    {
        // define style for all directories
        ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeDir, "", ImVec4(0.02f, 0.02f, 0.02f, 1.0f), ICON_MD_FOLDER);
        // define style for all files
        ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeFile, "", ImVec4(0.02f, .02f, 0.02f, 1.0f), ICON_IGFD_FILE);

#ifdef _WIN32
        wchar_t username[UNLEN + 1] = {0};
        DWORD username_len = UNLEN + 1;
        GetUserNameW(username, &username_len);
        std::wstring userPath = L"C:\\Users\\" + std::wstring(username) + L"\\";
        // Quick Access / Bookmarks
        ImGuiFileDialog::Instance()->AddBookmark(ICON_MD_MONITOR " Desktop", std::filesystem::path(userPath).append("Desktop").string());
        ImGuiFileDialog::Instance()->AddBookmark(ICON_MD_DESCRIPTION " Documents", std::filesystem::path(userPath).append("Documents").string());
        ImGuiFileDialog::Instance()->AddBookmark(ICON_MD_DOWNLOAD " Downloads", std::filesystem::path(userPath).append("Downloads").string());
        ImGuiFileDialog::Instance()->AddBookmark(ICON_MD_FAVORITE " Anim", std::filesystem::path("./").string());
#else
        // std::error_code ec;

        // // Quick Access
        // struct passwd *pw;
        // uid_t uid;
        // uid = geteuid();
        // pw = getpwuid(uid);
        // if (pw)
        // {
        //     std::string homePath = "/home/" + std::string(pw->pw_name);
        //     if (std::filesystem::exists(homePath, ec))
        //     if (std::filesystem::exists(homePath + "/Desktop", ec))
        //     if (std::filesystem::exists(homePath + "/Documents", ec))
        //     if (std::filesystem::exists(homePath + "/Downloads", ec))
        //     if (std::filesystem::exists(homePath + "/Pictures", ec))
        // }
#endif
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
        ImGuizmo::SetOrthographic(false); // is perspective
        ImGuizmo::BeginFrame();

        context_ = UiContext{};
        context_.menu.is_dialog_open = is_dialog_open_;
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
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        ImGui::Begin("DockSpace Demo", NULL, window_flags);

        ImGui::PopStyleVar(3);

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

    inline void InfosPane(const char *vFilter, IGFDUserDatas vUserDatas, bool *vCantContinue) // if vCantContinue is false, the user cant validate the dialog
    {
        ImGui::TextColored(ImVec4(0, 1, 1, 1), "Infos Pane");
        ImGui::Text("Linear: ");
        ImGui::SameLine();
        ImGui::Checkbox("##check", reinterpret_cast<bool *>(vUserDatas));
    }

    void MainLayer::draw_menu_bar(float fps)
    {
        const char *menu_dialog_name[4] = {
            "Import",
            "ImportDir",
            "Export",
            "ExportData"};
        std::array<bool *, 4> is_clicked_dir = {
            &context_.menu.clicked_import_model,
            &context_.menu.clicked_import_dir,
            &context_.menu.clicked_export_animation,
            &context_.menu.clicked_export_all_data};
        ImVec2 minSize = {650.0f, 400.0f}; // Half the display area
        const char *filters = FILTER_MODEL ",Json Animation (*.json){.json},.*";

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Import: model, animation", NULL, nullptr))
                {
                    is_dialog_open_ = true;
                    ImGuiFileDialog::Instance()->OpenDialog(menu_dialog_name[0],
                                                            ICON_MD_FILE_OPEN " Open fbx, gltf ...",
                                                            filters,
                                                            ".",
                                                            1,
                                                            nullptr,
                                                            ImGuiFileDialogFlags_Modal | ImGuiFileDialogFlags_DisableCreateDirectoryButton);
                }
                if (ImGui::MenuItem("Import: Folder", NULL, nullptr))
                {
                    is_dialog_open_ = true;
                    ImGuiFileDialog::Instance()->OpenDialog(menu_dialog_name[1], "Choose a Directory", nullptr, ".", 1, nullptr, ImGuiFileDialogFlags_Modal | ImGuiFileDialogFlags_DisableCreateDirectoryButton);
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Export: animation(selected model)", NULL, nullptr))
                {
                    is_dialog_open_ = true;
                    ImGuiFileDialog::Instance()->OpenDialog(menu_dialog_name[2], "Save",
                                                            FILTER_MODEL,
                                                            ".", "",
                                                            std::bind(&InfosPane, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), 150, 1,
                                                            IGFD::UserDatas(&isLinear),
                                                            ImGuiFileDialogFlags_Modal | ImGuiFileDialogFlags_ConfirmOverwrite);
                }
                if (ImGui::MenuItem("Export: rotation, world pos(json)", NULL, nullptr))
                {
                    is_dialog_open_ = true;
                    ImGuiFileDialog::Instance()->OpenDialog(menu_dialog_name[3], "Save",
                                                            "Json (*.json){.json}",
                                                            ".", 1, nullptr,
                                                            ImGuiFileDialogFlags_Modal | ImGuiFileDialogFlags_ConfirmOverwrite);
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
        int dialog_count = 0;
        for (int i = 0; i < 4; i++)
        {
            if (ImGuiFileDialog::Instance()->Display(menu_dialog_name[i], ImGuiWindowFlags_NoCollapse, minSize))
            {
                if (ImGuiFileDialog::Instance()->IsOk())
                {
                    context_.menu.path = ImGuiFileDialog::Instance()->GetFilePathName();
                    *is_clicked_dir[i] = true;
                }
                ImGuiFileDialog::Instance()->Close();
            }
            else
            {
                dialog_count++;
            }
        }
        if (dialog_count == 4)
        {
            is_dialog_open_ = false;
        }
        context_.menu.is_export_linear_interpolation = isLinear;
    }

    void MainLayer::draw_scene(const std::string &title, Scene *scene)
    {
        if (scene_layer_map_.find(title) == scene_layer_map_.end())
        {
            scene_layer_map_[title] = std::make_unique<SceneLayer>();
        }
        scene_layer_map_[title]->draw(title.c_str(), scene, context_);
    }

    void MainLayer::draw_component_layer(Scene *scene)
    {
        component_layer_.draw(context_.component, scene);
    }

    void MainLayer::draw_hierarchy_layer(Scene *scene)
    {
        hierarchy_layer_.draw(scene, context_);
    }
    void MainLayer::draw_timeline(Scene *scene)
    {
        timeline_layer_.draw(scene, context_);
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