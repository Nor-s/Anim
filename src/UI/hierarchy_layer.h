#ifndef UI_IMGUI_HIERARCHY_LAYER_H
#define UI_IMGUI_HIERARCHY_LAYER_H

#include <memory>
#include <imgui/imgui.h>

namespace glcpp
{
    struct ModelNode;
    class Animator;
}

namespace ui
{
    class HierarchyLayer
    {
    public:
        void draw(const glcpp::ModelNode *root_node, glcpp::Animator *animator);

    private:
        void draw_selected_node(const char *node_name, glcpp::Animator *animator);
        const char *dfs(const glcpp::ModelNode *anim_node, const ImGuiTreeNodeFlags &node_flags, int &count);
    };

}
#endif