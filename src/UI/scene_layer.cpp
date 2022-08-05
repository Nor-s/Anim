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

bool DecomposeTransform(const glm::mat4 &transform, glm::vec3 &translation, glm::vec3 &rotation, glm::vec3 &scale)
{
    // From glm::decompose in matrix_decompose.inl

    using namespace glm;
    using T = float;

    mat4 LocalMatrix(transform);

    // Normalize the matrix.
    if (epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), epsilon<T>()))
        return false;

    // First, isolate perspective.  This is the messiest.
    if (
        epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
        epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
        epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>()))
    {
        // Clear the perspective partition
        LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
        LocalMatrix[3][3] = static_cast<T>(1);
    }

    // Next take care of translation (easy).
    translation = vec3(LocalMatrix[3]);
    LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

    vec3 Row[3], Pdum3;

    // Now get scale and shear.
    for (length_t i = 0; i < 3; ++i)
        for (length_t j = 0; j < 3; ++j)
            Row[i][j] = LocalMatrix[i][j];

    // Compute X scale factor and normalize first row.
    scale.x = length(Row[0]);
    Row[0] = detail::scale(Row[0], static_cast<T>(1));
    scale.y = length(Row[1]);
    Row[1] = detail::scale(Row[1], static_cast<T>(1));
    scale.z = length(Row[2]);
    Row[2] = detail::scale(Row[2], static_cast<T>(1));

    // At this point, the matrix (in rows[]) is orthonormal.
    // Check for a coordinate system flip.  If the determinant
    // is -1, then negate the matrix and the scaling factors.
#if 0
		Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
		if (dot(Row[0], Pdum3) < 0)
		{
			for (length_t i = 0; i < 3; i++)
			{
				scale[i] *= static_cast<T>(-1);
				Row[i] *= static_cast<T>(-1);
			}
		}
#endif

    rotation.y = asin(-Row[0][2]);
    if (cos(rotation.y) != 0)
    {
        rotation.x = atan2(Row[1][2], Row[2][2]);
        rotation.z = atan2(Row[0][1], Row[0][0]);
    }
    else
    {
        rotation.x = atan2(-Row[2][0], Row[1][1]);
        rotation.z = 0;
    }

    return true;
}
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
        static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::OPERATION::ROTATE);
        static bool useSnap = true;
        float snap_value = 0.1f;
        float snap[3] = {snap_value, snap_value, snap_value};

        ImGuiIO &io = ImGui::GetIO();
        float viewManipulateRight = io.DisplaySize.x;
        float viewManipulateTop = 0;
        static ImGuiWindowFlags gizmoWindowFlags = 0;

        ImGui::Begin(title, 0, gizmoWindowFlags);

        // imguizmo setting
        ImGuizmo::SetDrawlist();
        float windowWidth = (float)ImGui::GetWindowWidth();
        float windowHeight = (float)ImGui::GetWindowHeight();
        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);
        viewManipulateRight = ImGui::GetWindowPos().x + windowWidth;
        viewManipulateTop = ImGui::GetWindowPos().y;
        ImGuiWindow *window = ImGui::GetCurrentWindow();
        gizmoWindowFlags = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(window->InnerRect.Min, window->InnerRect.Max) ? ImGuiWindowFlags_NoMove : 0;

        // scene camera
        auto &camera = scene->get_mutable_ref_camera();
        const float *const cameraView = static_cast<const float *const>(glm::value_ptr(camera->get_view()));
        const float *const cameraProjection = static_cast<const float *const>(glm::value_ptr(camera->get_projection()));

        // ImGuizmo::ViewManipulate(cameraView, camDistance, ImVec2(viewManipulateRight - 128, viewManipulateTop), ImVec2(128, 128), 0x10101010);

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
            }
        }

        ImGui::End();
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