#ifndef UI_IMGUI_TEXT_EDIT_H
#define UI_IMGUI_TEXT_EDIT_H

#include "imgui/imgui.h"
#include "imgui/TextEditor.h"
#include <filesystem>

class ImGuiTextEditor
{
public: // Load Fonts
        // (there is a default font, this is only if you want to change it. see extra_fonts/README.txt for more details)
        ///////////////////////////////////////////////////////////////////////
        // TEXT EDITOR SAMPLE
    TextEditor editor;
    TextEditor::LanguageDefinition lang = TextEditor::LanguageDefinition::CPlusPlus();

    // set your own known preprocessor symbols...
    const char *ppnames[1] = {"NULL"};
    // ... and their corresponding values
    const char *ppvalues[1] = {
        "#define NULL ((void*)0)"}; // set your own identifiers
    const char *identifiers[1] = {
        "HWND"};
    const char *idecls[1] =
        {"ssss"};
    std::string current_file_name_ = "";
    std::string modal_name_ = "editor";
    std::string current_file_path_ = "";
    bool is_save_open_ = false;

    void init()
    {
        for (int i = 0; i < sizeof(ppnames) / sizeof(ppnames[0]); ++i)
        {
            TextEditor::Identifier id;
            id.mDeclaration = ppvalues[i];
            lang.mPreprocIdentifiers.insert(std::make_pair(std::string(ppnames[i]), id));
        }

        for (int i = 0; i < sizeof(identifiers) / sizeof(identifiers[0]); ++i)
        {
            TextEditor::Identifier id;
            id.mDeclaration = std::string(idecls[i]);
            lang.mIdentifiers.insert(std::make_pair(std::string(identifiers[i]), id));
        }
        editor.SetLanguageDefinition(lang);
    }
    void open(const char *file_path)
    {
        auto path = std::filesystem::path(file_path);
        if (current_file_name_ == path.filename().string())
        {
            return;
        }
        std::ifstream t(file_path);
        if (t.good())
        {
            std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
            editor.SetText(str);
            current_file_name_ = path.filename().string();
            current_file_path_ = path.string();
            lang.mName = path.extension();
            editor.SetLanguageDefinition(lang);
        }
        if (t.is_open())
        {
            t.close();
        }
    }
    void quick_save(const std::string &str)
    {
        std::string path = "./tmp.json";
        if (current_file_path_ != "")
        {
            path = current_file_path_;
        }
        std::ofstream t(path.c_str());
        if (t.good())
        {
            t.write(str.c_str(), str.size());
        }
        if (t.is_open())
        {
            t.close();
        }
    }
    void save(const std::string &str)
    {
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        if (ImGui::BeginPopupModal("Save?", &is_save_open_, ImGuiWindowFlags_AlwaysAutoResize))
        {
            if (ImGui::Button("save", ImVec2(120, 0)))
            {
                quick_save(str);
                is_save_open_ = false;
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                is_save_open_ = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        else
        {
            std::cout << "fail\n";
        }
    }
    void draw(bool *p_open)
    {
        if (*p_open)
        {
            ImGuiWindowFlags window_flags = 0;
            window_flags = ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar;
            auto cpos = editor.GetCursorPosition();
            ImGui::Begin("Text Editor Demo", p_open, window_flags);
            ImGui::SetWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("Save"))
                    {
                        is_save_open_ = true;
                    }

                    if (ImGui::MenuItem("Quit"))
                    {
                        *p_open = false;
                    }
                    if (ImGui::MenuItem("Open"))
                    {
                        nfdchar_t *outPath;
                        nfdfilteritem_t filterItem[1] = {{"file", "glsl,json"}};
                        nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 1, NULL);

                        if (result == NFD_OKAY)
                        {
                            puts("Success!");
                            puts(outPath);
                            open(outPath);
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
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Edit"))
                {
                    bool ro = editor.IsReadOnly();
                    if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
                        editor.SetReadOnly(ro);
                    ImGui::Separator();

                    if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, !ro && editor.CanUndo()))
                        editor.Undo();
                    if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !ro && editor.CanRedo()))
                        editor.Redo();

                    ImGui::Separator();

                    if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, editor.HasSelection()))
                        editor.Copy();
                    if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && editor.HasSelection()))
                        editor.Cut();
                    if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && editor.HasSelection()))
                        editor.Delete();
                    if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
                        editor.Paste();

                    ImGui::Separator();

                    if (ImGui::MenuItem("Select all", nullptr, nullptr))
                        editor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(editor.GetTotalLines(), 0));

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("View"))
                {
                    if (ImGui::MenuItem("Dark palette"))
                        editor.SetPalette(TextEditor::GetDarkPalette());
                    if (ImGui::MenuItem("Light palette"))
                        editor.SetPalette(TextEditor::GetLightPalette());
                    if (ImGui::MenuItem("Retro blue palette"))
                        editor.SetPalette(TextEditor::GetRetroBluePalette());
                    ImGui::EndMenu();
                }

                ImGui::EndMenuBar();
            }
            if (ImGui::SmallButton("save"))
            {
                quick_save(editor.GetText());
            }
            ImGui::SameLine();
            ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, editor.GetTotalLines(),
                        editor.IsOverwrite() ? "Ovr" : "Ins",
                        editor.CanUndo() ? "*" : " ",
                        editor.GetLanguageDefinition().mName.c_str(), current_file_name_.c_str());

            editor.Render("TextEditor");

            if (is_save_open_)
            {
                if (!ImGui::IsPopupOpen("Save?"))
                    ImGui::OpenPopup("Save?");
                save(editor.GetText());
            }

            ImGui::End();
        }
    }
    void draw_to_modal(const char *path)
    {
        open(path);

        // ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        // ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        auto cpos = editor.GetCursorPosition();
        if (ImGui::BeginPopupModal(modal_name_.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_HorizontalScrollbar))

        {
            ImGui::SetWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
            ImGui::Text("All those beautiful files will be deleted.\nThis operation cannot be undone!\n\n");
            ImGui::Separator();

            ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, editor.GetTotalLines(),
                        editor.IsOverwrite() ? "Ovr" : "Ins",
                        editor.CanUndo() ? "*" : " ",
                        editor.GetLanguageDefinition().mName.c_str(), current_file_name_.c_str());

            editor.Render("TextEditor");
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
    const char *get_modal_name()
    {
        return modal_name_.c_str();
    }
};

#endif