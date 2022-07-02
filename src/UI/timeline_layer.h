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
    class Bone;
}

namespace ui
{
    class TextEditLayer;

    struct TimelineContext
    {
        bool is_clicked_play_all{false};
        bool is_clicked_play{false};
        bool is_clicked_stop{false};
        bool is_changed_animation{false};
        bool is_clicked_mp2mm{false};
        float fps = -1.0f;
        float tps = -1.0f;
        int animation_idx{-1};
    };

    class TimelineLayer
    {
    public:
        TimelineLayer();
        ~TimelineLayer() = default;
        void draw(Scene *scene, TimelineContext &context);

    private:
        void draw_play_all_button(TimelineContext &context);
        void draw_play_and_stop_button(TimelineContext &context);
        void draw_mp2mm(TimelineContext &context);
        void draw_animation_option_button(glcpp::AnimationComponent *animation_component);
        void draw_animation_list(TimelineContext &context, const SharedResources *shared_resources, const glcpp::Entity *entity);
        void draw_input_box(TimelineContext &context, const glcpp::AnimationComponent *animation_component);
        void draw_sequencer(TimelineContext &context, glcpp::AnimationComponent *animation_component);
        void draw_keyframes(glcpp::Animation *animation);
        void draw_keyframe_popup(TimelineContext &context);

        std::shared_ptr<TextEditLayer> text_editor_;
        bool is_hovered_zoom_slider_{false};
        uint32_t current_frame_{0u};
        uint32_t clicked_frame_{0u};
        float clicked_time_{-1.0f};
        glcpp::Bone *clicked_bone_ = nullptr;
        bool is_opened_transform_ = true;
    };
}

#endif