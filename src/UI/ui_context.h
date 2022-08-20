#ifndef ANIM_UI_UI_CONTEXT
#define ANIM_UI_UI_CONTEXT

#include <string>
#include <glm/glm.hpp>

namespace ui
{
    // Change animation
    struct ComponentContext
    {
        int current_animation_idx{-1};
        int new_animation_idx{-1};
    };

    // Change animator status (play/pause, fps, current time, start time, end time)
    struct TimelineContext
    {
        bool is_recording{false};
        bool is_clicked_play_back{false};
        bool is_clicked_play{false};
        bool is_stop{false};
        bool is_current_frame_changed{false};
        float fps{1.0f};
        int start_frame{0};
        int end_frame{200};
        int current_frame{0};
        bool is_forward{false};
        bool is_backward{false};
    };

    // import, export
    struct MenuContext
    {
        bool clicked_import_model{false};
        bool clicked_export_animation{false};
        std::string path{""};
    };

    // node change (bone, entity)
    struct EntityContext
    {
        bool is_changed_selected_entity{false};
        bool is_changed_transform{false};
        int selected_id{-1};
        // glm::mat4 before_transform;
        glm::mat4 new_transform{1.0f};
    };

    struct SceneContext
    {
        bool is_picking{false};
        bool is_bone_picking_mode{false};
        int x{0};
        int y{0};
    };

    struct UiContext
    {
        MenuContext menu{};
        TimelineContext timeline{};
        ComponentContext component{};
        EntityContext entity{};
        SceneContext scene{};
    };
}

#endif