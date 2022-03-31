#ifndef UI_IMGUI_SCENE_WINDOW_H
#define UI_IMGUI_SCENE_WINDOW_H

#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_glfw.h>
#include "scene/scene.hpp"
#include <memory>

class ImguiSceneWindow
{
public:
    void draw(const char *title, Scene *scene, bool is_static = false)
    {
        ImGui::Begin(title);
        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        width_ = viewportPanelSize.x;
        height_ = viewportPanelSize.y;
        is_hovered_ = (ImGui::IsWindowHovered()) ? true : false;
        before_height_ = height_;
        before_width_ = width_;
        // add rendered texture to ImGUI scene window
        float width = (float)scene->get_framebuffer()->get_width();
        float height = (float)scene->get_framebuffer()->get_height();

        if (is_static)
        {
            ImGui::Image(reinterpret_cast<void *>(scene->get_framebuffer()->get_color_texture()), ImVec2{width, height}, ImVec2{0, 1}, ImVec2{1, 0});
        }
        else
        {
            ImGui::Image(reinterpret_cast<void *>(scene->get_framebuffer()->get_color_texture()), ImVec2{width_, height_}, ImVec2{0, 1}, ImVec2{1, 0});
            if (width != width_ || height_ != height)
            {
                scene->set_size(width_, height_);
            }
        }

        ImGui::End();
    }
    float get_width()
    {
        return width_;
    }
    float get_height()
    {
        return height_;
    }
    float get_aspect()
    {
        return width_ / height_;
    }
    bool get_is_hovered()
    {
        return is_hovered_;
    }

private:
    bool is_hovered_ = false;
    float width_ = 800.0f;
    float before_width_ = 800.0f;
    float height_ = 800.0f;
    float before_height_ = 800.0f;
};

#endif
