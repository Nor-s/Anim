#ifndef UI_IMGUI_HIERARCHY_LAYER_H
#define UI_IMGUI_HIERARCHY_LAYER_H

#include <memory>
#include <imgui/imgui.h>

namespace anim
{
    class Model;
    class Animation;
    class Entity;
}

namespace ui
{
    class HierarchyLayer
    {
    public:
        HierarchyLayer();
        ~HierarchyLayer();
        void draw(anim::Entity *model);

    private:
        void draw_selected_node(const char *node_name, anim::Animation *animation);
        const char *dfs(const anim::ModelNode *anim_node, const ImGuiTreeNodeFlags &node_flags, int &count);
    };

}
#endif