#ifndef UI_IMGUI_TIMELINE_LAYER_H
#define UI_IMGUI_TIMELINE_LAYER_H

#include <vector>
#include <memory>

class Scene;

namespace ui
{
    class TextEditLayer;

    struct TimelineContext
    {
        uint32_t current_animation_idx;
        uint32_t clicked_frame{0};
        float clicked_time;
    };

    class TimelineLayer
    {
    public:
        TimelineLayer();
        ~TimelineLayer() = default;
        void draw(std::vector<std::shared_ptr<Scene>> &scenes, uint32_t scene_idx);

    private:
        void draw_sequencer();
        void draw_keyframes();
        void draw_keyframe_popup();
        void draw_play_all_button();
        void draw_play_and_stop_button();
        void draw_json_animation_button();
        void draw_animation_list();

        void draw_buttons();

        std::shared_ptr<TextEditLayer> text_editor_;
        // static bool IsHoveredZoomSlider = false;
        // static const char *CurrentAnimationName = nullptr;
    };
}

#endif