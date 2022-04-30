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
    std::string current_file = "";
    std::string modal_name_ = "editor";

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
        if(current_file == std::string(path.filename())) {
            return;
        }
        std::ifstream t(file_path);
        if (t.good())
        {
            std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
            editor.SetText(str);
            current_file = std::string(path.filename());
            lang.mName = path.extension();
            editor.SetLanguageDefinition(lang);
        }
    }
    void draw()
    {
        ImGuiWindowFlags window_flags = 0;
        window_flags = ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar;
        auto cpos = editor.GetCursorPosition();
        ImGui::Begin("Text Editor Demo", nullptr, window_flags);
        ImGui::SetWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Save"))
                {
                    auto textToSave = editor.GetText();
                    /// save text....
                }
                if (ImGui::MenuItem("Quit", "Alt-F4"))
                {
                }
                if (ImGui::MenuItem("Open", "Alt-F1"))
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
        ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, editor.GetTotalLines(),
                    editor.IsOverwrite() ? "Ovr" : "Ins",
                    editor.CanUndo() ? "*" : " ",
                    editor.GetLanguageDefinition().mName.c_str(), current_file.c_str());

        editor.Render("TextEditor");
        ImGui::End();
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
                        editor.GetLanguageDefinition().mName.c_str(), current_file.c_str());

            editor.Render("TextEditor");
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        
    }const char* get_modal_name() {
            return modal_name_.c_str();
        }
};

#endif