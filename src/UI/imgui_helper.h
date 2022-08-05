#ifndef UI_IMGUI_HELPER_H
#define UI_IMGUI_HELPER_H

#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <string>
namespace ui
{
    void HelpMarker(const char *desc);
    bool DragPropertyXYZ(const char *label, glm::vec3 &value, float step = 0.01f, float min = -1000.0f, float max = 1000.0f, const std::string &help_message = "");
}

#endif