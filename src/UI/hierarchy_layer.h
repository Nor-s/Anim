#ifndef UI_IMGUI_HIERARCHY_LAYER_H
#define UI_IMGUI_HIERARCHY_LAYER_H

#include <memory>
#include <imgui/imgui.h>

namespace glcpp
{
    class Model;
    class Animation;
    struct ModelNode;
}

namespace ui
{
    class HierarchyLayer
    {
    public:
        HierarchyLayer();
        ~HierarchyLayer();
        void draw(glcpp::Model *model);

    private:
        void draw_selected_node(const char *node_name, glcpp::Animation *animation);
        const char *dfs(const glcpp::ModelNode *anim_node, const ImGuiTreeNodeFlags &node_flags, int &count);
    };

}
#endif