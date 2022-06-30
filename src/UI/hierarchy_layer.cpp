#include "hierarchy_layer.h"

#include "glcpp/model.h"
#include "glcpp/entity.h"
#include "glcpp/anim/animation.hpp"
#include "glcpp/anim/bone.hpp"
#include "glcpp/component/animation_component.h"

#include <glm/glm.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace ui
{
    HierarchyLayer::HierarchyLayer() = default;
    HierarchyLayer::~HierarchyLayer() = default;
    void HierarchyLayer::draw(glcpp::Entity *entity)
    {
        static const char *selected_node_name = nullptr;
        auto model = entity->get_mutable_model();
        ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
        int node_count = 0;
        ImGui::Begin("Hierarchy");
        {
            if (model)
            {
                auto root_node = model->get_root_node();
                auto animation = entity->get_mutable_pointer_animation_component();
                auto node_name = dfs(root_node, node_flags, node_count);
                if (node_name)
                {
                    selected_node_name = node_name;
                }
                if (selected_node_name && animation)
                {
                    draw_selected_node(selected_node_name, animation->get_mutable_animation());
                }
            }
            else
            {
                ImGui::Text("No model selected");
            }
        }
        ImGui::End();
    }
    void HierarchyLayer::draw_selected_node(const char *node_name, glcpp::Animation *animation)
    {
        ImGui::BeginChild("Bone property", {0, 100}, true);
        ImGui::Text("%s", node_name);
        glcpp::Bone *bone = animation->FindBone(node_name);
        if (bone)
        {
            glm::vec3 *p_pos = bone->get_mutable_pointer_recently_used_position();
            glm::quat *p_quat = bone->get_mutable_pointer_recently_used_rotation();
            glm::vec3 *p_scale = bone->get_mutable_pointer_recently_used_scale();
            if (p_pos)
            {
                ImGui::SliderFloat3("position", &(*p_pos)[0], -10.0f, 10.0f);
            }
            if (p_quat)
            {
                auto before_quat = *p_quat;
                ImGui::SliderFloat4("quaternion", &(*p_quat)[0], -1.0f, 1.0f);
                if (before_quat != *p_quat)
                {
                    *p_quat = glm::normalize(*p_quat);
                }
            }
            if (p_scale)
            {
                ImGui::SliderFloat3("scale", &(*p_scale)[0], 10.0f, -10.0f);
            }
        }
        else
        {
            ImGui::Text("Can't find bone");
        }
        ImGui::EndChild();
    }

    const char *HierarchyLayer::dfs(const glcpp::ModelNode *node, const ImGuiTreeNodeFlags &node_flags, int &count)
    {
        static int selected_idx = 0;
        const char *selected_node = nullptr;
        auto selected_flags = node_flags;
        count++;
        if (selected_idx == count)
        {
            selected_flags |= ImGuiTreeNodeFlags_Selected;
        }
        if (node->childrens.size() == 0)
        {
            selected_flags |= ImGuiTreeNodeFlags_Leaf;
        }
        bool node_open = ImGui::TreeNodeEx(node->name.c_str(), selected_flags);
        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
        {
            selected_idx = count;
            selected_node = node->name.c_str();
        }

        if (node_open)
        {
            for (size_t i = 0; i < node->childrens.size(); i++)
            {
                const char *selected_child_node = dfs(node->childrens[i].get(), node_flags, count);
                if (selected_child_node != nullptr)
                {
                    selected_node = selected_child_node;
                }
            }
            ImGui::TreePop();
        }
        return selected_node;
    }
}