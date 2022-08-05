#include "imgui_helper.h"
#include <imgui/imgui_internal.h>

namespace ui
{
    void HelpMarker(const char *desc)
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
    bool DragPropertyXYZ(const char *label, glm::vec3 &value, float step, float min, float max, const std::string &help_message)
    {
        bool is_value_changed = false;
        ImGui::PushID(label);
        ImGui::Columns(2, nullptr, false);
        ImGui::SetColumnWidth(0, 80.0f);

        ImGui::Text(label);
        ImGui::NextColumn();

        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0.f, 0.f});
        // X
        {
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.f});
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.9f, 0.2f, 0.2f, 1.f});
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.8f, 0.1f, 0.15f, 1.f});
            ImGui::Button("X");
            ImGui::PopStyleColor(4);

            ImGui::SameLine();
            is_value_changed |= ImGui::DragFloat("##x", &value.x, step, min, max);
            ImGui::PopItemWidth();
            ImGui::SameLine();
        }

        // Y
        {
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.f});
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.3f, 0.8f, 0.3f, 1.f});
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.2f, 0.7f, 0.2f, 1.f});
            ImGui::Button("Y");
            ImGui::PopStyleColor(4);

            ImGui::SameLine();
            is_value_changed |= ImGui::DragFloat("##y", &value.y, step, min, max);
            ImGui::PopItemWidth();
            ImGui::SameLine();
        }

        // Z
        {
            // Z
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1f, 0.25f, 0.8f, 1.f});
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.2f, 0.35f, 0.9f, 1.f});
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.1f, 0.25f, 0.8f, 1.f});
            ImGui::Button("Z");
            ImGui::PopStyleColor(4);

            ImGui::SameLine();
            is_value_changed |= ImGui::DragFloat("##z", &value.z, step, min, max);
            ImGui::PopItemWidth();
        }

        ImGui::PopStyleVar();
        ImGui::Columns(1);

        ImGui::PopID();

        return is_value_changed;
    }
}
