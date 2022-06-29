#ifndef UI_IMGUI_TIMELINE_LAYER_H
#define UI_IMGUI_TIMELINE_LAYER_H

#include <vector>
#include <memory>

class Scene;

namespace ui
{
    class TextEditLayer;
    class TimelineLayer
    {
    public:
        TimelineLayer();
        ~TimelineLayer() = default;
        void draw(std::vector<std::shared_ptr<Scene>> &scenes, uint32_t scene_idx);

    private:
        void draw_play_all_button();
        void draw_play_and_stop_button();
        void draw_json_animation_button();

        void draw_buttons();
        std::shared_ptr<TextEditLayer> text_editor_;
    };
}

#endif