#include "scene_layer.h"
#include "scene/scene.hpp"
#include "glcpp/framebuffer.h"

#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_glfw.h>
#include <memory>

namespace ui
{

    void SceneLayer::draw(const char *title, Scene *scene)
    {
        ImGui::Begin(title);
        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        width_ = viewportPanelSize.x;
        height_ = viewportPanelSize.y;

        is_hovered_ = (ImGui::IsWindowFocused() && ImGui::IsWindowHovered()) ? true : false;
        // add rendered texture to ImGUI scene window
        float width = (float)scene->get_mutable_framebuffer()->get_width();
        float height = (float)scene->get_mutable_framebuffer()->get_height();

        ImGui::Image(reinterpret_cast<void *>(static_cast<intptr_t>(scene->get_mutable_framebuffer()->get_color_texture())), ImVec2{width_, height_}, ImVec2{0, 1}, ImVec2{1, 0});
        if (width != width_ || height_ != height)
        {
            scene->set_size(width_, height_);
        }

        ImGui::End();
    }
    float SceneLayer::get_width()
    {
        return width_;
    }
    float SceneLayer::get_height()
    {
        return height_;
    }
    float SceneLayer::get_aspect()
    {
        return width_ / height_;
    }
    bool SceneLayer::get_is_hovered()
    {
        return is_hovered_;
    }
}