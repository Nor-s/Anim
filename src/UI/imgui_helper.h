#ifndef UI_IMGUI_HELPER_H
#define UI_IMGUI_HELPER_H

#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <string>
namespace ui
{
    inline ImVec2 SumImVec2(const ImVec2 &a, const ImVec2 &b);
    inline ImVec2 SubImVec2(const ImVec2 &a, const ImVec2 &b);
    void HelpMarker(const char *desc);
    bool DragPropertyXYZ(const char *label, glm::vec3 &value, float step = 0.01f, float min = -1000.0f, float max = 1000.0f, const std::string &help_message = "");
    int ToggleButton(const char *label, bool *v, const ImVec2 &size = {0.0f, 0.0f}, const ImVec4 &col_inactive_btn = {0.4f, 0.4f, 0.4f, 0.8f}, const ImVec4 &col_inactive_btn_hovered = {0.2f, 0.4f, 0.6f, 0.8f});

}

#endif