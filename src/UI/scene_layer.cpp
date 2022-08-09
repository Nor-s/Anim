#include "scene_layer.h"
#include "imgui_helper.h"

#include "scene/scene.hpp"
#include "glcpp/framebuffer.h"
#include "glcpp/camera.h"
#include "glcpp/entity.h"
#include "glcpp/utility.hpp"
#include "glcpp/component/transform_component.h"

#include <memory>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui_internal.h>
#include <imgui/icons/icons.h>

namespace ui
{
    SceneLayer::SceneLayer()
        : width_(800.0f),
          height_(800.0f),
          is_hovered_(false),
          is_select_mode_(true),
          is_univ_mode_(false),
          is_rotate_mode_(false),
          is_scale_mode_(false),
          is_translate_mode_(false),
          scene_window_right_(0.0f),
          scene_window_top_(0.0f),
          scene_pos_(0.0f, 0.0f),
          current_gizmo_mode_(ImGuizmo::LOCAL),
          current_gizmo_operation_(ImGuizmo::OPERATION::NONE)
    {
    }
    void SceneLayer::draw(const char *title, Scene *scene)
    {
        static ImGuiWindowFlags sceneWindowFlags = 0;

        // add rendered texture to ImGUI scene window
        float width = (float)scene->get_mutable_framebuffer()->get_width();
        float height = (float)scene->get_mutable_framebuffer()->get_height();
        // draw scene window
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0.0f, 0.0f});
        if (ImGui::Begin(title, 0, sceneWindowFlags | ImGuiWindowFlags_NoScrollbar))
        {
            scene_pos_ = ImGui::GetWindowPos();
            ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
            width_ = viewportPanelSize.x;
            height_ = viewportPanelSize.y;
            ImGuiWindow *window = ImGui::GetCurrentWindow();
            is_hovered_ = !ImGuizmo::IsOver() && ImGui::IsWindowFocused() && ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(window->InnerRect.Min, window->InnerRect.Max);

            // draw scene framebuffer
            ImGui::Image(reinterpret_cast<void *>(static_cast<intptr_t>(scene->get_mutable_framebuffer()->get_color_texture())), ImVec2{width_, height_}, ImVec2{0, 1}, ImVec2{1, 0});
            if (width != width_ || height_ != height)
            {
                scene->set_size(width_, height_);
            }
            draw_gizmo(scene);
            sceneWindowFlags = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(window->InnerRect.Min, window->InnerRect.Max) ? ImGuiWindowFlags_NoMove : 0;
        }
        ImGui::End();
        ImGui::PopStyleVar();

        draw_mode_window();
    }
    void SceneLayer::draw_gizmo(Scene *scene)
    {
        bool useSnap = true;
        float snap_value = 0.1f;
        float snap[3] = {snap_value, snap_value, snap_value};

        ImGuiIO &io = ImGui::GetIO();
        scene_window_right_ = io.DisplaySize.x;
        scene_window_top_ = 0;
        // imguizmo setting
        ImGuizmo::SetDrawlist();
        ImGuizmo::SetRect(scene_pos_.x, scene_pos_.y, width_, height_);
        scene_window_right_ = scene_pos_.x + width_;
        scene_window_top_ = scene_pos_.y;

        // scene camera
        auto &camera = scene->get_mutable_ref_camera();
        float *cameraView = const_cast<float *>(glm::value_ptr(camera->get_view()));
        float *cameraProjection = const_cast<float *>(glm::value_ptr(camera->get_projection()));
        auto *selected_entity = scene->get_mutable_selected_entity();

        if (!is_select_mode_ && selected_entity)
        {
            auto &transform = selected_entity->get_mutable_transform();
            glm::mat4 object_matrix = transform.get_mat4();
            float *object_mat_ptr = static_cast<float *>(glm::value_ptr(object_matrix));

            ImGuizmo::Manipulate(cameraView,
                                 cameraProjection,
                                 current_gizmo_operation_,
                                 current_gizmo_mode_,
                                 object_mat_ptr,
                                 NULL,
                                 useSnap ? &snap[0] : NULL);
            if (ImGuizmo::IsUsing())
            {
                auto [tt, r, s] = glcpp::DecomposeTransform(glm::make_mat4(object_mat_ptr));

                glm::vec3 t = glm::abs(transform.get_translation() - tt);
                if (!(t.x > 500.0f || t.y > 500.0f || t.z > 500.0f))
                {
                    glm::vec3 euler = glm::eulerAngles(r);

                    transform.set_translation(tt)
                        .set_scale(s)
                        .set_rotation(euler);
                }
            }
            if (ImGuizmo::IsOver())
            {
                is_hovered_ = false;
            }
        }
        ImGuizmo::ViewManipulate(cameraView, 8.0f, ImVec2{scene_window_right_ - 128.0f, scene_window_top_ + 16.0f}, ImVec2{128, 128}, ImU32{0x10101010});
    }
    void SceneLayer::draw_mode_window()
    {
        ImGuiIO &io = ImGui::GetIO();
        ImVec2 mode_window_size = {512.0f, 65.0f};
        ImVec2 mode_window_pos = {scene_window_right_ - width_ * 0.5f - mode_window_size.x * 0.5f, scene_window_top_ + height_ - 50.0f};
        if (height_ - mode_window_size.y < 5.0f || width_ - mode_window_size.x < 5.0f)
        {
            mode_window_pos = {-1000.0f, -1000.0f};
        }

        ImGui::SetNextWindowSize(mode_window_size);
        ImGui::SetNextWindowPos(mode_window_pos);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, {55.0f, 65.0f});
        if (ImGui::Begin("Child", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground))
        {

            ImVec2 btn_size{80.0f, 60.0f};

            if (ImGui::BeginTable("split", 1, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_BordersOuterV | ImGuiTableFlags_NoPadInnerX | ImGuiTableFlags_SizingFixedSame))
            {
                current_gizmo_operation_ = ImGuizmo::OPERATION::NONE;
                ImGui::TableNextColumn();
                // select
                bool before_select_mode = is_select_mode_;
                ToggleButton(ICON_MD_NEAR_ME, &is_select_mode_, btn_size);
                if (!before_select_mode && is_select_mode_)
                {
                    is_univ_mode_ = false;
                    is_rotate_mode_ = false;
                    is_scale_mode_ = false;
                    is_translate_mode_ = false;
                }
                ImGui::SameLine();
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0.f, 0.f});
                // all
                bool before_univ_mode = is_univ_mode_;
                ToggleButton(ICON_MD_FORMAT_SHAPES, &is_univ_mode_, btn_size);
                if (is_univ_mode_)
                {
                    is_scale_mode_ = is_rotate_mode_ = is_translate_mode_ = true;
                    current_gizmo_operation_ = current_gizmo_operation_ | ImGuizmo::OPERATION::UNIVERSAL;
                }
                if (before_univ_mode && !is_univ_mode_)
                {
                    is_scale_mode_ = is_rotate_mode_ = is_translate_mode_ = false;
                }
                ImGui::SameLine();
                // move
                ImGui::PushFont(io.Fonts->Fonts[ICON_FA]);
                ToggleButton(ICON_FA_ARROWS_UP_DOWN_LEFT_RIGHT, &is_translate_mode_, btn_size);
                if (is_translate_mode_)
                {
                    current_gizmo_operation_ = current_gizmo_operation_ | ImGuizmo::OPERATION::TRANSLATE;
                }
                ImGui::PopFont();
                ImGui::SameLine();
                // rotate
                ToggleButton(ICON_MD_FLIP_CAMERA_ANDROID, &is_rotate_mode_, btn_size);
                if (is_rotate_mode_)
                {
                    current_gizmo_operation_ = current_gizmo_operation_ | ImGuizmo::OPERATION::ROTATE;
                }
                ImGui::SameLine();
                // scale
                ToggleButton(ICON_MD_PHOTO_SIZE_SELECT_SMALL, &is_scale_mode_, btn_size);
                if (is_scale_mode_)
                {
                    current_gizmo_operation_ = current_gizmo_operation_ | ImGuizmo::OPERATION::SCALE;
                }

                ImGui::PopStyleVar();
                ImGui::SameLine();
                // skeleton
                static bool is_selected = false;
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0.f, 0.f});
                ToggleButton(ICON_MD_PERSON_OFF, &is_selected, btn_size);

                is_select_mode_ = is_select_mode_ && !(is_univ_mode_ || is_rotate_mode_ || is_scale_mode_ || is_translate_mode_);

                if (is_select_mode_)
                {
                    current_gizmo_operation_ = ImGuizmo::OPERATION::NONE;
                }
                if (is_univ_mode_ && !(is_rotate_mode_ && is_scale_mode_ && is_translate_mode_))
                {
                    is_univ_mode_ = false;
                    is_rotate_mode_ = !is_rotate_mode_;
                    is_scale_mode_ = !is_scale_mode_;
                    is_translate_mode_ = !is_translate_mode_;
                }

                ImGui::EndTable();
                ImGui::PopStyleVar();
            }
        }
        ImGui::End();
        ImGui::PopStyleVar();
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