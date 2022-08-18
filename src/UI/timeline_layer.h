#ifndef UI_IMGUI_TIMELINE_LAYER_H
#define UI_IMGUI_TIMELINE_LAYER_H

#include <vector>
#include <memory>
#include <string>

class Scene;

namespace anim
{
    class Entity;
    class SharedResources;
    class Animation;
    class Animator;
    class AnimationComponent;
    class Bone;
}

namespace ui
{
    class TextEditLayer;

    struct TimelineContext
    {
        bool is_recording{false};
        bool is_clicked_play_back{false};
        bool is_clicked_play{false};
        bool is_clicked_bone{false};
        bool is_stop{false};
        bool is_current_frame_changed{false};
        float fps{1.0f};
        int start_frame{0};
        int end_frame{200};
        int current_frame{0};
        bool is_forward{false};
        bool is_backward{false};
        std::string clicked_bone_name{""};
    };

    class TimelineLayer
    {
    public:
        TimelineLayer();
        ~TimelineLayer() = default;
        void draw(Scene *scene, TimelineContext &context);

    private:
        inline void init_context(TimelineContext &context, Scene *scene);
        void draw_animator_status(TimelineContext &context);
        // TODO: Fix neo sequencer error when start frame is not 0"
        void draw_sequencer(TimelineContext &context);
        void draw_keyframes(TimelineContext &context, const anim::Animation *animation);
        void draw_keyframe_popup(TimelineContext &context);
        void draw_bone_status();

        std::shared_ptr<TextEditLayer> text_editor_;
        bool is_hovered_zoom_slider_{false};
        uint32_t current_frame_{0u};
        uint32_t clicked_frame_{0u};
        float clicked_time_{-0.0f};
        anim::Bone *clicked_bone_ = nullptr;
        bool is_opened_transform_ = false;
        Scene *scene_;
        anim::Entity *entity_;
        anim::SharedResources *resources_;
        anim::Animator *animator_;
    };
}

#endif