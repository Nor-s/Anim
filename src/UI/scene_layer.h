#ifndef UI_IMGUI_SCENE_WINDOW_H
#define UI_IMGUI_SCENE_WINDOW_H

#include "scene/scene.hpp"

namespace ui
{
    class SceneLayer
    {
    public:
        SceneLayer() = default;
        ~SceneLayer() = default;
        void draw(const char *title, Scene *scene);
        float get_width();
        float get_height();
        float get_aspect();
        bool get_is_hovered();

    private:
        bool is_hovered_ = false;
        float width_ = 800.0f;
        float height_ = 800.0f;
    };
}

#endif
