#ifndef UI_IMGUI_MENU_BAR_LAYER_H
#define UI_IMGUI_MENU_BAR_LAYER_H

#include <string>
#include <memory>
#include <map>

struct GLFWwindow;

class Scene;

namespace ui
{
    class HierarchyLayer;
    class ModelPropertyLayer;
    class SceneLayer;
    class TimelineLayer;
    /**
     * @brief dock + menu bar(import, export)
     *
     */
    class MainLayer
    {
    public:
        MainLayer();
        ~MainLayer();
        void init(GLFWwindow *window);
        void begin();
        void end();
        void draw();

    private:
        void init_layer();
        void shutdown();
        void draw_dock(bool *p_open);
        void draw_scene(const std::string &title, Scene *scene);

        std::map<std::string, std::unique_ptr<SceneLayer>> scene_layer_map_;
        std::unique_ptr<HierarchyLayer> hierarchy_layer_;
        std::unique_ptr<ModelPropertyLayer> property_layer_;
        std::unique_ptr<TimelineLayer> timeline_layer_;
    };
}

#endif