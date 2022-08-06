#include "scene_layer.h"
#include "scene/scene.hpp"
#include "glcpp/framebuffer.h"
#include "glcpp/camera.h"
#include "glcpp/entity.h"
#include "glcpp/utility.hpp"
#include "glcpp/component/transform_component.h"

#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/ImGuizmo.h>
#include <memory>
#include <imgui/imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui_internal.h"
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/icons/icons.h>

namespace ui
{
    static const float identityMatrix[16] =
        {1.f, 0.f, 0.f, 0.f,
         0.f, 1.f, 0.f, 0.f,
         0.f, 0.f, 1.f, 0.f,
         0.f, 0.f, 0.f, 1.f};
    float objectMatrix[4][16] = {
        {100.f, 0.f, 0.f, 0.f,
         0.f, 100.f, 0.f, 0.f,
         0.f, 0.f, 100.f, 0.f,
         0.f, 0.f, 0.f, 1.f},

        {1.f, 0.f, 0.f, 0.f,
         0.f, 1.f, 0.f, 0.f,
         0.f, 0.f, 1.f, 0.f,
         2.f, 0.f, 0.f, 1.f},

        {1.f, 0.f, 0.f, 0.f,
         0.f, 1.f, 0.f, 0.f,
         0.f, 0.f, 1.f, 0.f,
         2.0f, 0.f, 2.f, 1.f},

        {1.f, 0.f, 0.f, 0.f,
         0.f, 1.f, 0.f, 0.f,
         0.f, 0.f, 1.f, 0.f,
         0.f, 0.f, 2.f, 1.f}};
    void SceneLayer::draw(const char *title, Scene *scene)
    {
        static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
        static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::OPERATION::ROTATE | ImGuizmo::OPERATION::TRANSLATE | ImGuizmo::OPERATION::SCALE | ImGuizmo::OPERATION::UNIVERSAL);
        static bool useSnap = true;
        float snap_value = 0.1f;
        float snap[3] = {snap_value, snap_value, snap_value};

        ImGuiIO &io = ImGui::GetIO();
        float viewManipulateRight = io.DisplaySize.x;
        float viewManipulateTop = 0;
        static ImGuiWindowFlags gizmoWindowFlags = 0;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0.0f, 0.0f});
        ImGui::Begin(title, 0, gizmoWindowFlags | ImGuiWindowFlags_NoScrollbar);

        // imguizmo setting
        ImGuizmo::SetDrawlist();
        float windowWidth = (float)ImGui::GetWindowWidth();
        float windowHeight = (float)ImGui::GetWindowHeight();
        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);
        viewManipulateRight = ImGui::GetWindowPos().x + windowWidth;
        viewManipulateTop = ImGui::GetWindowPos().y;
        ImGuiWindow *window = ImGui::GetCurrentWindow();
        gizmoWindowFlags = (ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(window->InnerRect.Min, window->InnerRect.Max)) ? ImGuiWindowFlags_NoMove : 0;

        // scene camera
        auto &camera = scene->get_mutable_ref_camera();
        float *cameraView = const_cast<float *>(glm::value_ptr(camera->get_view()));
        float *cameraProjection = const_cast<float *>(glm::value_ptr(camera->get_projection()));

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        width_ = viewportPanelSize.x;
        height_ = viewportPanelSize.y;
        is_hovered_ = !ImGuizmo::IsOver() && ImGui::IsWindowFocused() && ImGui::IsWindowHovered();
        // add rendered texture to ImGUI scene window
        float width = (float)scene->get_mutable_framebuffer()->get_width();
        float height = (float)scene->get_mutable_framebuffer()->get_height();

        ImGui::Image(reinterpret_cast<void *>(static_cast<intptr_t>(scene->get_mutable_framebuffer()->get_color_texture())), ImVec2{width_, height_}, ImVec2{0, 1}, ImVec2{1, 0});
        if (width != width_ || height_ != height)
        {
            scene->set_size(width_, height_);
        }
        auto *selected_entity = scene->get_mutable_selected_entity();
        if (selected_entity)
        {
            auto &transform = selected_entity->get_mutable_transform();
            glm::mat4 object_matrix = transform.get_mat4();
            float *object_mat_ptr = static_cast<float *>(glm::value_ptr(object_matrix));

            ImGuizmo::Manipulate(cameraView,
                                 cameraProjection,
                                 mCurrentGizmoOperation,
                                 mCurrentGizmoMode,
                                 object_mat_ptr,
                                 NULL,
                                 useSnap ? &snap[0] : NULL);
            if (ImGuizmo::IsUsing())
            {
                auto [t, r, s] = glcpp::DecomposeTransform(glm::make_mat4(object_mat_ptr));
                glm::vec3 euler = glm::eulerAngles(r);

                transform.set_translation(t)
                    .set_scale(s)
                    .set_rotation(euler);
                is_hovered_ = false;
            }
            if (ImGuizmo::IsOver())
            {
                is_hovered_ = false;
            }
        }
        ImGuizmo::ViewManipulate(cameraView, 8.0f, ImVec2{viewManipulateRight - 128.0f, viewManipulateTop + 16.0f}, ImVec2{128, 128}, ImU32{0x10101010});

        ImGui::PopStyleVar();
        // ImGui::SetCursorScreenPos({ui_width, start_y});
        ImGui::End();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, {55.0f, 65.0f});
        ImVec2 mode_window_size = {512.0f, 65.0f};
        ImVec2 mode_window_pos = {viewManipulateRight - windowWidth * 0.5f - mode_window_size.x * 0.5f, viewManipulateTop + height_ - 50.0f};
        if (height_ - mode_window_size.y < 5.0f || width_ - mode_window_size.x < 5.0f)
        {
            mode_window_pos = {-1000.0f, -1000.0f};
        }
        ImGui::SetNextWindowSize(mode_window_size);
        ImGui::SetNextWindowPos(mode_window_pos);
        // ImGui::SetNextWindowPos();
        if (ImGui::Begin("Child", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground))
        {

            ImVec2 btn_size{80.0f, 60.0f};

            if (ImGui::BeginTable("split", 1, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_BordersOuterV | ImGuiTableFlags_NoPadInnerX | ImGuiTableFlags_SizingFixedSame))
            {
                ImGui::TableNextColumn();
                // select
                ImGui::PushStyleColor(ImGuiCol_Button, {0.4f, 0.4f, 0.4f, 0.8f});
                ImGui::Button(ICON_MD_NEAR_ME, btn_size);
                ImGui::SameLine();
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0.f, 0.f});
                // move
                {
                    ImGui::PushFont(io.Fonts->Fonts[ICON_FA]);
                    ImGui::Button(ICON_FA_ARROWS_UP_DOWN_LEFT_RIGHT, btn_size);
                    ImGui::PopFont();
                }
                ImGui::SameLine();
                // rotate

                ImGui::Button(ICON_MD_FLIP_CAMERA_ANDROID, btn_size);
                ImGui::SameLine();
                // scale
                ImGui::Button(ICON_MD_PHOTO_SIZE_SELECT_SMALL, btn_size);
                ImGui::SameLine();
                // all
                ImGui::Button(ICON_MD_FORMAT_SHAPES, btn_size);
                ImGui::PopStyleVar();
                ImGui::SameLine();
                // skeleton
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0.f, 0.f});
                ImGui::Button(ICON_MD_PERSON_OFF, btn_size);
                ImGui::PopStyleColor();

                ImGui::EndTable();
                ImGui::PopStyleVar();
            }
            ImGui::End();
        }
        ImGui::PopStyleVar();
        ImGui::ShowDemoWindow();
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