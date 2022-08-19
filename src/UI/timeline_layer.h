#ifndef UI_IMGUI_TIMELINE_LAYER_H
#define UI_IMGUI_TIMELINE_LAYER_H

#include <vector>
#include <memory>
#include <string>
#include "ui_context.h"

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

    class TimelineLayer
    {
    public:
        TimelineLayer();
        ~TimelineLayer() = default;
        void draw(Scene *scene, UiContext &context);

    private:
        inline void init_context(UiContext &ui_context, Scene *scene);
        void draw_animator_status(UiContext &ui_context);
        // TODO: Fix neo sequencer error when start frame is not 0"
        void draw_sequencer(UiContext &ui_context);
        void draw_keyframes(UiContext &ui_context, const anim::Animation *animation);
        // void draw_keyframe_popup(UiContext &ui_context);
        // void draw_bone_status(UiContext &ui_context);

        std::shared_ptr<TextEditLayer> text_editor_;
        bool is_hovered_zoom_slider_{false};
        uint32_t current_frame_{0u};
        uint32_t clicked_frame_{0u};
        float clicked_time_{-0.0f};
        anim::Bone *clicked_bone_ = nullptr;
        bool is_opened_transform_ = true;
        Scene *scene_;
        anim::Entity *root_entity_;
        anim::Entity *entity_;
        anim::SharedResources *resources_;
        anim::Animator *animator_;
    };
}

#endif