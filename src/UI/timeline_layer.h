#ifndef UI_IMGUI_TIMELINE_LAYER_H
#define UI_IMGUI_TIMELINE_LAYER_H

#include <vector>
#include <memory>

class Scene;
class SharedResources;

namespace glcpp
{
    class Entity;
    class Animation;
    class AnimationComponent;
}

namespace ui
{
    class TextEditLayer;

    struct TimelineContext
    {
        uint32_t clicked_frame{0u};
        float clicked_time{0.0f};
        bool is_clicked_play_all{false};
        bool is_clicked_play{false};
        bool is_clicked_stop{false};
        bool is_changed_animation{false};
        uint32_t animation_idx{0u};
    };

    class TimelineLayer
    {
    public:
        TimelineLayer();
        ~TimelineLayer() = default;
        void draw(Scene *scene, TimelineContext &context);

    private:
        void draw_sequencer(glcpp::Animation *current_animation);
        void draw_keyframes();
        void draw_keyframe_popup();
        void draw_play_all_button(TimelineContext &context);
        void draw_play_and_stop_button(TimelineContext &context);
        void draw_animation_option_button(glcpp::Entity *entity);
        void draw_animation_list(TimelineContext &context, const SharedResources *shared_resources, const glcpp::Entity *entity);

        std::shared_ptr<TextEditLayer> text_editor_;
        static bool IsHoveredZoomSlider;
        // static const char *CurrentAnimationName = nullptr;
    };
}

#endif