#include "main_layer.h"
#include "ui_context.h"
#include "imgui_helper.h"
#include "scene_layer.h"

#include "text_edit_layer.h"

#include <array>
#include <glcpp/window.h>

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>

#include "imgui_impl_sdl.h"
#include <SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif
#include <imgui/ImGuizmo.h>
#include <imgui/icons/icons.h>
#include <fstream>
#include <sstream>

#include <ImGuiFileDialog/ImGuiFileDialog.h>
#ifdef _WIN32
#pragma warning(disable : 4005)
#include <Windows.h>
// #include <shellapi.h>
#include <lmcons.h>
// #pragma comment(lib, "Shell32.lib")
#else
#include <unistd.h>
#include <pwd.h>
#endif

#define FILTER_MODEL "Model files (*.fbx *.gltf){.fbx,.gltf,.glb}"

namespace ui
{
    static bool isLinear{true};
    static float ImportScale{100.0f};

    MainLayer::MainLayer() = default;

    MainLayer::~MainLayer()
    {
        shutdown();
    }

    void MainLayer::init(SDL_Window *window, void* context)
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
        ImGui_ImplSDL2_InitForOpenGL(window, context);
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
#elif __APPLE__
        std::string user_name;
        user_name = "/Users/" + std::string(getenv("USER"));
        std::string homePath = user_name;
        if (std::filesystem::exists(homePath + "/Desktop"))
        {
            ImGuiFileDialog::Instance()->AddBookmark(ICON_MD_MONITOR " Desktop", std::filesystem::path(homePath + "/Desktop").string());
        }
        if (std::filesystem::exists(homePath + "/Documents"))
        {
            ImGuiFileDialog::Instance()->AddBookmark(ICON_MD_DESCRIPTION " Documents", homePath + "/Documents");
        }
        if (std::filesystem::exists(homePath + "/Downloads"))
        {
            ImGuiFileDialog::Instance()->AddBookmark(ICON_MD_DOWNLOAD " Downloads", homePath + "/Downloads");
        }
        ImGuiFileDialog::Instance()->AddBookmark(ICON_MD_FAVORITE " Anim", std::filesystem::path("./").string());
#endif
        std::ifstream wif("./bookmark");
        if (wif.good())
        {
            std::stringstream ss;
            ss << wif.rdbuf();
            ImGuiFileDialog::Instance()->DeserializeBookmarks(ss.str());
        }
        if (wif.is_open())
        {
            wif.close();
        }
    }

    void MainLayer::shutdown()
    {
        std::ofstream bookmark_stream("./bookmark");
        std::string bookmark = ImGuiFileDialog::Instance()->SerializeBookmarks(false);
        bookmark_stream << bookmark;
        if (bookmark_stream.is_open())
        {
            bookmark_stream.close();
        }

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }

    void MainLayer::begin()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        ImGuizmo::SetOrthographic(false); // is perspective
        ImGuizmo::BeginFrame();

        context_ = UiContext{};
    }

    void MainLayer::end()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        ImGuiIO &io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
            SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            SDL_GL_MakeCurrent(backup_current_window, backup_current_context);

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

    inline void LinearInfosPane(const char *vFilter, IGFDUserDatas vUserDatas, bool *vCantContinue) // if vCantContinue is false, the user cant validate the dialog
    {
        ImGui::TextColored(ImVec4(0, 1, 1, 1), "Export");
        ImGui::Text("Linear: ");
        ImGui::SameLine();
        ImGui::Checkbox("##check", reinterpret_cast<bool *>(vUserDatas));
    }
    // TODO: IMPORT OPTION: SCALE
    // inline void ScaleInfosPane(const char *vFilter, IGFDUserDatas vUserDatas, bool *vCantContinue) // if vCantContinue is false, the user cant validate the dialog
    // {
    //     ImGui::TextColored(ImVec4(0, 1, 1, 1), "Import");
    //     ImGui::Text("Scale: ");
    //     ImGui::SameLine();
    //     ImGui::DragFloat("##check", reinterpret_cast<float *>(vUserDatas), 1.0f, 1.0f, 200.0f);
    // }
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
        static bool py_modal = false;

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Import: model, animation", NULL, nullptr))
                {
                    ImGuiFileDialog::Instance()->OpenDialog(menu_dialog_name[0],
                                                            ICON_MD_FILE_OPEN " Open fbx, gltf ...",
                                                            filters,
                                                            ".", 1, nullptr,
                                                            // std::bind(&ScaleInfosPane, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), 150, 1,
                                                            // IGFD::UserDatas(&ImportScale),
                                                            ImGuiFileDialogFlags_Modal | ImGuiFileDialogFlags_DisableCreateDirectoryButton);
                }
                if (ImGui::MenuItem("Import: Folder", NULL, nullptr))
                {
                    ImGuiFileDialog::Instance()->OpenDialog(menu_dialog_name[1], "Choose a Directory",
                                                            nullptr,
                                                            ".", 1, nullptr,
                                                            // std::bind(&ScaleInfosPane, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), 150, 1,
                                                            // IGFD::UserDatas(&ImportScale),
                                                            ImGuiFileDialogFlags_Modal | ImGuiFileDialogFlags_DisableCreateDirectoryButton);
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Export: animation(selected model)", NULL, nullptr))
                {
                    ImGuiFileDialog::Instance()->OpenDialog(menu_dialog_name[2], "Save",
                                                            FILTER_MODEL,
                                                            ".", "",
                                                            std::bind(&LinearInfosPane, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), 150, 1,
                                                            IGFD::UserDatas(&isLinear),
                                                            ImGuiFileDialogFlags_Modal | ImGuiFileDialogFlags_ConfirmOverwrite);
                }
                // for data extract (for deep learning)
                // if (ImGui::MenuItem("Export: rotation, world pos(json)", NULL, nullptr))
                // {
                //     ImGuiFileDialog::Instance()->OpenDialog(menu_dialog_name[3], "Save",
                //                                             "Json (*.json){.json}",
                //                                             ".", 1, nullptr,
                //                                             ImGuiFileDialogFlags_Modal | ImGuiFileDialogFlags_ConfirmOverwrite);
                // }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Python"))
            {
                if (ImGui::MenuItem("Mediapipe", NULL, nullptr))
                {
                    py_modal = true;
                }

                ImGui::EndMenu();
            }

            ImGui::Text("fps: %.2f", fps);

            ImGui::EndMenuBar();
        }
        draw_python_modal(py_modal);
        int dialog_count = 0;
        for (int i = 0; i < 3; i++)
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
        }
        if (ImGuiFileDialog::Instance()->IsOpened())
        {
            context_.menu.is_dialog_open = true;
        }
        context_.menu.is_export_linear_interpolation = isLinear;
        context_.menu.import_scale = ImportScale;
    }
    void MainLayer::draw_python_modal(bool &is_open)
    {
        static std::string video_path = "";
        static std::string save_path = std::filesystem::absolute("./animation.json").string();
        video_path.resize(200);
        save_path.resize(200);
        if (is_open)
        {
            ImGui::OpenPopup("Mediapipe");
            is_open = false;
        }
        ImGuiStyle &style = ImGui::GetStyle();
        auto color = style.Colors[ImGuiCol_Button];
        color.x = 1.0f - color.x;
        color.y = 1.0f - color.y;
        color.z = 1.0f - color.z;
        ImGui::PushStyleColor(ImGuiCol_FrameBg, color);
        if (ImGui::BeginPopupModal("Mediapipe", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            context_.menu.is_dialog_open = true;
            ImGui::Text("You must have a selected model.");
            ImGui::Text("Video:");
            ImGui::SameLine();
            auto text_cursor = ImGui::GetCursorPosX();
            char *path = video_path.data();
            ImGui::InputText("##video_path", path, video_path.size());
            ImGui::SameLine();
            auto current_cursor = ImGui::GetCursorPosX();
            if (ImGui::Button("Open"))
            {
                ImGuiFileDialog::Instance()->OpenDialog("ChooseVideo", "Choose a Video",
                                                        "Video (*.mp4 *.gif){.mp4,.gif,.avi},.*",
                                                        ".", 1, nullptr,
                                                        ImGuiFileDialogFlags_Modal | ImGuiFileDialogFlags_DisableCreateDirectoryButton);
            }
            if (ImGuiFileDialog::Instance()->Display("ChooseVideo", ImGuiWindowFlags_NoCollapse, {650.0f, 400.0f}))
            {
                if (ImGuiFileDialog::Instance()->IsOk())
                {
                    video_path = ImGuiFileDialog::Instance()->GetFilePathName();
                }
                ImGuiFileDialog::Instance()->Close();
            }

            ImGui::Text("Save:");
            ImGui::SameLine(text_cursor);
            char *s_path = save_path.data();
            ImGui::InputText("##save_path", s_path, save_path.size());
            ImGui::SameLine(current_cursor);
            if (ImGui::Button("Open##2"))
            {
                ImGuiFileDialog::Instance()->OpenDialog("ChooseJson", "Choose a Json",
                                                        "JSON (*.json){.json},.*",
                                                        ".", 1, nullptr,
                                                        ImGuiFileDialogFlags_Modal | ImGuiFileDialogFlags_DisableCreateDirectoryButton);
            }
            if (ImGuiFileDialog::Instance()->Display("ChooseJson", ImGuiWindowFlags_NoCollapse, {650.0f, 400.0f}))
            {
                if (ImGuiFileDialog::Instance()->IsOk())
                {
                    save_path = ImGuiFileDialog::Instance()->GetFilePathName();
                }
                ImGuiFileDialog::Instance()->Close();
            }
            ImGui::Separator();
            ImGui::Text("Factor");
            ImGui::NewLine();
            ImGui::SameLine(text_cursor);
            ImGui::DragFloat("##factor", &context_.python.factor, 0.1f, 0.0f, 10.0f);
            ImGui::Text("FPS");
            ImGui::NewLine();
            ImGui::SameLine(text_cursor);
            ImGui::DragFloat("##fps", &context_.python.fps, 1.0f, 0.0f, 144.0f);
            ImGui::Text("Visibility");
            ImGui::NewLine();
            ImGui::SameLine(text_cursor);
            ImGui::DragFloat("##visibility", &context_.python.min_visibility, 0.1f, 0.0f, 1.0f);
            ImGui::Text("Angle Adjustment");
            ImGui::NewLine();
            ImGui::SameLine(text_cursor);
            ImGui::Checkbox("##is_angle_adjustment", &context_.python.is_angle_adjustment);
            ImGui::Separator();
            ImGui::Text("Model Complexity");
            ImGui::NewLine();
            ImGui::SameLine(text_cursor);
            ImGui::DragInt("##model_complexity", &context_.python.model_complexity, 1, 0, 1);
            ImGui::Text("Model Min Detection Confidence");
            ImGui::NewLine();
            ImGui::SameLine(text_cursor);
            ImGui::DragFloat("##detection", &context_.python.min_detection_confidence, 0.1f, 0.1f, 1.0f);
            if (ImGui::Button("OK"))
            {
                ImGui::CloseCurrentPopup();
                context_.python.video_path = video_path;
                context_.python.save_path = save_path;
                context_.python.is_clicked_convert_btn = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("Close"))
                ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }
        else
        {
            context_.menu.is_dialog_open = false;
        }
        ImGui::PopStyleColor();
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
    void MainLayer::process_events(SDL_Event& event) {
        ImGui_ImplSDL2_ProcessEvent(&event);
    }
}