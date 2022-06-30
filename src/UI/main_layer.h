#ifndef UI_IMGUI_MENU_BAR_LAYER_H
#define UI_IMGUI_MENU_BAR_LAYER_H

#include <string>
#include <memory>
#include <map>
struct GLFWwindow;

class Scene;
class SharedResources;

namespace glcpp
{
    class Entity;
}

namespace ui
{
    class HierarchyLayer;
    class ModelPropertyLayer;
    class SceneLayer;
    class TimelineLayer;

    struct MenuContext
    {
        bool clicked_import_model{false};
        bool clicked_export_animation{false};
    };

    struct UiContext
    {
        MenuContext menu_context;
    };
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
        void draw_dock(float fps);
        void draw_scene(const std::string &title, Scene *scene);
        void draw_model_properties(glcpp::Entity *entity);
        void draw_hierarchy_layer(glcpp::Entity *entity);

        bool is_scene_layer_hovered(const std::string &title);
        const UiContext &get_context() const;

    private:
        void init_layer();
        void shutdown();
        void draw_menu_bar(float fps);
        // https://www.fluentcpp.com/2017/09/22/make-pimpl-using-unique_ptr/
        std::map<std::string, std::unique_ptr<SceneLayer>> scene_layer_map_;
        std::unique_ptr<HierarchyLayer> hierarchy_layer_;
        std::unique_ptr<ModelPropertyLayer> property_layer_;
        std::unique_ptr<TimelineLayer> timeline_layer_;
        UiContext context_;
    };
}

#endif