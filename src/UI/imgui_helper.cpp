#include "imgui_helper.h"
#include <imgui/imgui_internal.h>
#include <iostream>

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
        ImGui::SetColumnWidth(0, 90.0f);

        ImGui::Text(label);
        ImGui::NextColumn();

        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0.f, 0.f});

        const char *button_labels[] = {"X", "Y", "Z"};
        const char *drag_labels[] = {"##x", "##y", "##z"};

        for (int i = 0; i < 3; i++)
        {
            auto button_color = glm::vec4{0.0f, 0.0f, 0.0f, 0.5f};
            auto framebg_color = glm::vec4{0.4f, 0.4f, 0.4f, 0.1f};
            auto framebg_hovered_color = glm::vec4{0.4f, 0.4f, 0.4f, 0.7f};
            auto framebg_active_color = glm::vec4{0.5f, 0.5f, 0.5f, 1.0f};

            button_color[i] = framebg_hovered_color[i] = framebg_active_color[i] = framebg_color[i] = 0.9f;
            ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(button_color.r * 255, button_color.g * 255, button_color.b * 255, button_color.a * 255));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(button_color.r * 255, button_color.g * 255, button_color.b * 255, button_color.a * 255));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(button_color.r * 255, button_color.g * 255, button_color.b * 255, button_color.a * 255));
            ImGui::PushStyleColor(ImGuiCol_FrameBgActive, IM_COL32(framebg_active_color.r * 255, framebg_active_color.g * 255, framebg_active_color.b * 255, framebg_active_color.a * 255));
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(framebg_hovered_color.r * 255, framebg_hovered_color.g * 255, framebg_hovered_color.b * 255, framebg_hovered_color.a * 255));
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
            ImGui::Button(button_labels[i]);
            ImGui::PopStyleColor(1);

            ImGui::SameLine();
            is_value_changed |= ImGui::DragFloat(drag_labels[i], &value[i], step, min, max);
            ImGui::PopItemWidth();
            if (i != 3)
            {
                ImGui::SameLine();
            }
            ImGui::PopStyleColor(5);
        }

        ImGui::PopStyleVar();
        ImGui::Columns(1);

        ImGui::PopID();

        return is_value_changed;
    }
}
